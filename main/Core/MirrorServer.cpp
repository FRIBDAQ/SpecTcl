/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  MirrorServer.cpp
 *  @brief:  Implement the methods of the MirrorServer class.
 */
#include "MirrorServer.h"
#include "CSocket.h"
#include "MirrorMessages.h"
#include "MirrorDirectory.h"
#include <stdexcept>
//////////////////////////////////////////////////////////////////////////////
// MirrorServer implementation

/**
 * MirrorServer - constructor
 *
 *  @param pListener - the server listener passed to base class ctor.
 *  @param pSocket   - Socket we communicate on (passed to bse class ctor).
 *  @param pMemory   - Pointer to the memory we serve.
 */
MirrorServer::MirrorServer(
    ServerListener* pListener, CSocket* pSocket, Xamine_shared* pMemory
) :
    AbstractClientServer(pListener, pSocket),
    m_sendAll(true),
    m_pMemory(pMemory)
{}

/**
 * onConnection
 *    Get the peer information and put it in the m_peer member. Once we get
 *    a Mirror::MSG_TYPE_SHMINFO message, we can enter ourselves in the mirror
 *    directory.
 * @param pSocket - socket we're connected to.
 * @param pData   - unused client data.
 */
void
MirrorServer::onConnection(CSocket* pSocket, ClientData pData)
{
    // CSocket likes to throw if e.g. client immediately exited...
    try {
        unsigned short port;
        pSocket->getPeer(port, m_peer);
    } catch(...) {
        pSocket->Shutdown();
    }
}
/**
 * onReadable
 *     Called when the socket is readable
 *     process messages:
 *     -  Read the header.
 *     -  Call the appropriate handler.
 *     @note all inside a try/catch block in case the socket closes on us.
 * @param pSocket - the socket
 * @param pData   - Unused client data.
 */
void
MirrorServer::onReadable(CSocket* pSocket, ClientData pData)
{
    try {
        // Read the header and if it's a runt, shutdown the socket.
        
        Mirror::MessageHeader  hdr;
        int s = pSocket->Read(&hdr, sizeof(hdr));
        if (s != sizeof(hdr)) {
            pSocket->Shutdown();
            return;
        }
        // Dispatch - an error also shuts down
        
        int remaining = hdr.s_messageSize - sizeof(Mirror::MessageHeader);
        
        if (hdr.s_messageType == Mirror::MSG_TYPE_SHMINFO) {
            processMemoryInfo(pSocket, remaining);    
        } else if (hdr.s_messageType == Mirror::MSG_TYPE_REQUEST_UPDATE) {
            processUpdate(pSocket, remaining);
        } else {
            // Bad message type:
            
            pSocket->Shutdown();
            return;
        }
        
    } catch (...) {
        pSocket->Shutdown();        // Socket problems -> shutdown.
    }
}
/**
 * onClose
 *    Remove ourself from the directory, if we got there.
 * @param pSocket - socket that closed.
 * @param pData   - Ignored client data.
 */
void
MirrorServer::onClose(CSocket* pSocket, ClientData pData)
{
    MirrorDirectorySingleton* pDir = MirrorDirectorySingleton::getInstance();
    pDir->remove(m_peer);
}
/**
 * requestFullUpdate
 *     Sets the flag that says the next update must be full.
 */
void
MirrorServer::requestFullUpdate()
{
    m_sendAll = true;
}
///////////////////////////////////////////////////////////////////////////////
// Private utilities.
//

/**
 * spectrumDataPointer
 *    Returns a  pointer to where spectrum data start in the shared memory
 *
 * @return void*
 */
void*
MirrorServer::spectrumDataPointer()
{
    return m_pMemory->dsp_spectra.XAMINE_b;
}
/**
 * usedSpectrumBytes
 *   @return size_t  number of bytes of used spectrum memory.
 *   @note there can be holes in the data so we look for the maximum offset then
 *   figure out how big that is...we can only send a contiguous chunk to the client
 *   so this size includes holes as well.
 *   @note if there are no defined spectra 1 is returned anyway.
 */
size_t
MirrorServer::usedSpectrumBytes() const
{
    int idx(-1);
    size_t maxOffset(0);
    
    for (int i =0; i < XAMINE_MAXSPEC; i++) {
        spec_type tp = m_pMemory->dsp_types[i];
        if (tp != undefined) {
            // We are going to convert the offset which is in spectrum data units
            // to a byte offset as that's what we'll eventually need anyway:
            
            size_t bOffset = m_pMemory->dsp_offsets[i];
            if ((tp == twodlong) || (tp == onedlong)) {
                bOffset *= sizeof(uint32_t);
            } else if ((tp == onedword) || (tp == twodword)) {
                bOffset *= sizeof(uint16_t);
            }    // byte needs no scaling.
            
            if (bOffset > maxOffset) {
                idx = i;
                maxOffset = bOffset;
            }
        }
    }
    // If idx == -1, there are no spectra:
    
    if (idx == -1) {
        return 1;
    }
    // Figure out the size of the spectrum in bytes, that added to maxOffset
    // are the number of bytes of storage.
    
    size_t nBytes(0);
    size_t xdim = m_pMemory->dsp_xy[idx].xchans;
    size_t ydim = m_pMemory->dsp_xy[idx].ychans;
    switch (m_pMemory->dsp_types[idx]) {
    twodlong:
        nBytes = xdim*ydim * sizeof(uint32_t);
        break;
    twodword:
        nBytes = xdim*ydim * sizeof(uint16_t);
        break;
    twodbyte:
        nBytes = xdim*ydim;
        break;
    onedlong:
        nBytes = xdim * sizeof(uint32_t);
        break;
    onedword:
        nBytes = xdim * sizeof(uint16_t);
        break;
    }
    return maxOffset + nBytes;
    
}
/**
 * headerSize
 *    Returns the size of the header that describes spectra in shared memory
 * @return size_t
 *
 */
size_t
MirrorServer::headerSize() const
{
    return offsetof(Xamine_shared, dsp_spectra);
}
/**
 * processMemoryInfo
 *    We should have a memory info block following.  This implies that
 *    We read that in and make a directory entry for our host.
 *    A duplicate entry will result in an exception which will make the
 *    caller shut things down.
 * @param pSocket - socket on which we read/write.
 * @param msgSize - number of remaining message bytes --- according to sender.
 * @note no reply is sent back.  Success is indicted by a continued connection.
 */
void
MirrorServer::processMemoryInfo(CSocket* pSocket, uint32_t msgSize)
{
    if (msgSize != sizeof(Mirror::MemoryKey)) {
        throw std::logic_error("Invalid memory key message size sent by client");
    }
    Mirror::MemoryKey key;                     // This is an array.
    size_t s = pSocket->Read(key, sizeof(key));
    if (s != sizeof(key)) {
        throw std::logic_error("Invalid read size for memory key");
    }
    std::string strKey;
    for (int i = 0; i < sizeof(Mirror::MemoryKey); i++) {
        strKey.push_back(key[i]);
    }
    MirrorDirectorySingleton* pDir = MirrorDirectorySingleton::getInstance();
    pDir->put(m_peer, strKey);

}
/**
 * processUpdate
 *    Called when the message type is  Mirror::MSG_TYPE_REQUEST_UPDATE
 *     If the m_sendAll  flag is set we send the headers and spectrum memory,
 *     otherwise we send only the spectrum memory.
 * @param pSocket - socket on which we send the reply.
 * @param msgSize - Bytes remaining in the message - should be zero.
 */
void
MirrorServer::processUpdate(CSocket* pSocket, uint32_t msgSize)
{
    if (msgSize) {
        throw std::logic_error("Request update body is not zero length");
    }
    size_t sendSize = usedSpectrumBytes();
    void* pBase;
    Mirror::MessageHeader hdr;
    
    if (m_sendAll) {
        pBase = m_pMemory;
        sendSize    = headerSize();
        m_sendAll = false;
        hdr.s_messageType = Mirror::MSG_TYPE_FULL_UPDATE;

    } else {
        pBase = spectrumDataPointer();
        hdr.s_messageType = Mirror::MSG_TYPE_PARTIAL_UPDATE;
    }
    hdr.s_messageSize = sendSize + sizeof(hdr);
    
    // Now we can send the data:
    
    try {
        int nSent = pSocket->Write(&hdr, sizeof(hdr));
        if (nSent != sizeof(hdr)) {
            throw std::logic_error("Failed to send update message header");
        }
        nSent = pSocket->Write(pBase, sendSize);
        if (nSent != sendSize) {
            throw std::logic_error("Failed to send the whole update message body.");
        }
    } catch (...) {
        pSocket->Shutdown();
        return;
    }
}
//////////////////////////////////////////////////////////////////////////////
// MirrorServerFactory implementation.

/**
 * MirrorServerFactory - constructor.
 *   @param pMemory - pointer to Xamine shared memory that will be served out.
 */
MirrorServerFactory::MirrorServerFactory(Xamine_shared* pMemory) :
    m_pMemory(pMemory)
{

}
/**
 * create
 *    Create a new instance of a server thread.  Note the thread must be
 *    started by the caller.
 *  @param pSocket - socket along which the client and server will communicate.
 *  @param pListener - The Server listener thread.
 *  @param cd        - Client data, which is ignored.
 *  @return AbstractClientServer* Pointer to a dynamically server thread object.
 */
AbstractClientServer*
MirrorServerFactory::create(
    CSocket* pSocket, ServerListener* pListener, ClientData cd
)
{
    return new MirrorServer(pListener, pSocket, m_pMemory);
}