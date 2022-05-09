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

/** @file:  mservertests.cpp
 *  @brief: Tests for the mirror server.
 *  @note there's absolutely no reason we have to operate with an actual
 *        shared memory region.  Any memory block will do.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "xamineDataTypes.h"
#include "MirrorServer.h"
#include "MirrorMessages.h"
#include "MirrorDirectory.h"
#include "AbstractThreadedServer.h"
#include "CSocket.h"

#include <string.h>
#include <unistd.h>


int currentPort = 5555;
class mservertest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(mservertest);
    CPPUNIT_TEST(connect_1);
    CPPUNIT_TEST(connect_2);
    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(get_2);
    CPPUNIT_TEST(get_3);
    
    CPPUNIT_TEST(spectrum_1);
    CPPUNIT_TEST(spectrum_2);
    CPPUNIT_TEST(spectrum_3);
    CPPUNIT_TEST(spectrum_4);
    CPPUNIT_TEST(spectrum_5);
    CPPUNIT_TEST(spectrum_6);
    CPPUNIT_TEST(spectrum_7);
    CPPUNIT_TEST_SUITE_END();
    
private:
    Xamine_shared m_memory;
    ServerListener* m_listener;
    MirrorServerFactory* m_factory;
public:
    void setUp() {
        // No defined spectra.
        
        for (int i =0; i < XAMINE_MAXSPEC; i++) {
            m_memory.dsp_types[i]  = undefined;
        }
        m_factory = new MirrorServerFactory(&m_memory);
	auto sport = std::to_string(currentPort);
        m_listener = new ServerListener(sport.c_str(), m_factory);
        m_listener->start();
    }
    void tearDown() {
        if (m_listener) {
            m_listener->requestExit();
            m_listener->join();
        }
        delete m_listener;
        delete m_factory;
	currentPort++;
    }
protected:
    void connect_1();
    void connect_2();
    void get_1();
    void get_2();
    void get_3();
    
    void spectrum_1();
    void spectrum_2();
    void spectrum_3();
    void spectrum_4();
    void spectrum_5();
    void spectrum_6();
    void spectrum_7();
    
};

CPPUNIT_TEST_SUITE_REGISTRATION(mservertest);

// Can connect to the server.
void mservertest::connect_1()
{
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));
    CPPUNIT_ASSERT_NO_THROW(client.Shutdown());
}
void mservertest::connect_2()
{
    // If I connect and send the memory desription message, I'll get
    // that into the directory singleton.
    
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));
    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Wait for the client to run:
    
    sleep(1);
    
    MirrorDirectorySingleton* p = MirrorDirectorySingleton::getInstance();
    auto listing = p->list();
    EQ(size_t(1), listing.size());
    EQ(std::string("ABCD"), listing[0].second);
    
    client.Shutdown();
}
// The first get should be a full get.
// Since we're not defining any spectra, we get the header and one byte
// of spectrum storage only.
//
void mservertest::get_1()
{
    // Connect and be kosher by sending our 'memory key'.
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the header and validate the type:
    
    int s = client.Read(&update, sizeof(update));
    EQ(size_t(s), sizeof(update));
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    size_t dataSize = offsetof(Xamine_shared, dsp_spectra) + 1;
    size_t totalSize= sizeof(update) + dataSize;
    EQ(totalSize, size_t(update.s_messageSize));
    
    char data[dataSize];
    s = client.Read(data, dataSize);
    EQ(dataSize, size_t(s));
    
    client.Shutdown();
}
// The get of the description items gets us the correct data
// byte for byte:

void mservertest::get_2()
{
    // Connect and be kosher by sending our 'memory key'.
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));
    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the header and validate the type:
    
    int s = client.Read(&update, sizeof(update));
    EQ(size_t(s), sizeof(update));
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    size_t dataSize = offsetof(Xamine_shared, dsp_spectra) + 1;  
    size_t totalSize= sizeof(update) + dataSize ;
    EQ(totalSize, size_t(update.s_messageSize));
    
    char data[dataSize];
    s = client.Read(data, dataSize);
    EQ(dataSize, size_t(s));
    
    EQ(0, memcmp(&m_memory, data, dataSize));
    
    client.Shutdown();
}
// The second get only gives us the data which, in this case,
// with no spectra  defined is just a token byte:

void
mservertest::get_3()
{
    // Connect and be kosher by sending our 'memory key'.
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the full update.
    
    int s = client.Read(&update, sizeof(update));
    
    size_t dataSize = offsetof(Xamine_shared, dsp_spectra);
    size_t totalSize= sizeof(update) + dataSize;
    
    char data[dataSize];
    s = client.Read(data, dataSize);
    
    // Next request will give a partial update with only a token byte:
    
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();

    s = client.Read(&update, sizeof(update));
    EQ(Mirror::MSG_TYPE_PARTIAL_UPDATE, update.s_messageType);
    EQ(sizeof(update) + 1, size_t(update.s_messageSize));
    
    s = client.Read(data, 1);
    EQ(1, s);
    
    
    client.Shutdown();
}

// Define a 1d spectrum.  THe first get should show that definition
// and the spectrum.   The second should get the data of the spectrum.


void
mservertest::spectrum_1()
{
    m_memory.dsp_xy[0].xchans = 256;
    m_memory.dsp_xy[0].ychans = 0;
    m_memory.dsp_offsets[0] = 0;
    m_memory.dsp_types[0] = onedlong;
    
    uint32_t* pSpectrum = m_memory.dsp_spectra.XAMINE_l;
    for (int i = 0; i < 256; i++) {
        *pSpectrum++ = i;
    }
    
    // Ok now that we're set up get a connection and request an update.
    // The first one will have 256*sizeof(uint32_t) + sizeof(xamine header).
    // bytes of payload.  The second should just have 256*sizeof(uint32_t)
    // bytes of data.  The spectrum should be 256 channels of counting
    // pattern in both cases.
    
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the full update header and be sure it's what's expected.
    
    int s = client.Read(&update, sizeof(update));
    
    size_t expectedSpectrumSize = 256*sizeof(uint32_t);
    size_t expectedHeaderSize   = offsetof(Xamine_shared, dsp_spectra);
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    EQ(
       sizeof(update) + expectedSpectrumSize + expectedHeaderSize,
       size_t(update.s_messageSize)
    );
    // Read/validate the data:
    
    static Xamine_shared mirror;   // So as not to run into stack limits.
    s = client.Read(&mirror, expectedSpectrumSize + expectedHeaderSize);
    EQ(expectedSpectrumSize + expectedHeaderSize, size_t(s));
    EQ(m_memory.dsp_xy[0].xchans, mirror.dsp_xy[0].xchans);
    EQ(m_memory.dsp_xy[0].ychans, mirror.dsp_xy[0].ychans);
    EQ(m_memory.dsp_offsets[0], mirror.dsp_offsets[0]);
    EQ(m_memory.dsp_types[0], mirror.dsp_types[0]);
    
    for (int i =0; i < 256; i++) {
        EQ(m_memory.dsp_spectra.XAMINE_l[i], mirror.dsp_spectra.XAMINE_l[i]);
        
    }
    
    // The next should just read the spectra.
    
    memset(&mirror, 0, sizeof(mirror));
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Should be a partial update with expectedSpectrumSize payload:
    
    s = client.Read(&update, sizeof(update));
    EQ(Mirror::MSG_TYPE_PARTIAL_UPDATE, update.s_messageType);
    EQ(expectedSpectrumSize + sizeof(update), size_t(update.s_messageSize));
    
    s = client.Read(&mirror.dsp_spectra.XAMINE_l, expectedSpectrumSize);
    EQ(expectedSpectrumSize, size_t(s));
    for (int i =0; i < 256; i++) {
        EQ(m_memory.dsp_spectra.XAMINE_l[i], mirror.dsp_spectra.XAMINE_l[i]);
        
    }
    
    client.Shutdown();
    
}
// 2- D Spectrum.

void mservertest::spectrum_2()
{
    m_memory.dsp_xy[0].xchans = 256;
    m_memory.dsp_xy[0].ychans = 256;
    m_memory.dsp_offsets[0] = 0;
    m_memory.dsp_types[0] = twodlong;
    
    uint32_t* pSpectrum = m_memory.dsp_spectra.XAMINE_l;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++)
        *pSpectrum++ = i+j;
    }
    
    // Ok now that we're set up get a connection and request an update.
    // The first one will have 256*sizeof(uint32_t) + sizeof(xamine header).
    // bytes of payload.  The second should just have 256*sizeof(uint32_t)
    // bytes of data.  The spectrum should be 256 channels of counting
    // pattern in both cases.
    
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the full update header and be sure it's what's expected.
    
    int s = client.Read(&update, sizeof(update));
    
    size_t expectedSpectrumSize = 256*256*sizeof(uint32_t);
    size_t expectedHeaderSize   = offsetof(Xamine_shared, dsp_spectra);
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    EQ(
       sizeof(update) + expectedSpectrumSize + expectedHeaderSize,
       size_t(update.s_messageSize)
    );
    // Read/validate the data:
    
    static Xamine_shared mirror;   // So as not to run into stack limits.
    s = client.Read(&mirror, expectedSpectrumSize + expectedHeaderSize);
    EQ(expectedSpectrumSize + expectedHeaderSize, size_t(s));
    EQ(m_memory.dsp_xy[0].xchans, mirror.dsp_xy[0].xchans);
    EQ(m_memory.dsp_xy[0].ychans, mirror.dsp_xy[0].ychans);
    EQ(m_memory.dsp_offsets[0], mirror.dsp_offsets[0]);
    EQ(m_memory.dsp_types[0], mirror.dsp_types[0]);
    
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            256*256*sizeof(uint32_t)
        )
    );
    
    
    // The next should just read the spectra.
    
    memset(&mirror, 0, sizeof(mirror));
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Should be a partial update with expectedSpectrumSize payload:
    
    s = client.Read(&update, sizeof(update));
    EQ(Mirror::MSG_TYPE_PARTIAL_UPDATE, update.s_messageType);
    EQ(expectedSpectrumSize + sizeof(update), size_t(update.s_messageSize));
    
    s = client.Read(&mirror.dsp_spectra.XAMINE_l, expectedSpectrumSize);
    EQ(expectedSpectrumSize, size_t(s));
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            256*256*sizeof(uint32_t)
        )
    );
    
    
    client.Shutdown();
    
}
// Two spectra but no storage holes in the spectrum soup.

void
mservertest::spectrum_3()
{
    m_memory.dsp_xy[0].xchans = 256;
    m_memory.dsp_xy[0].ychans = 0;
    m_memory.dsp_offsets[0] = 0;
    m_memory.dsp_types[0] = onedlong;
    
    uint32_t* pSpectrum = m_memory.dsp_spectra.XAMINE_l;
    for (int i = 0; i < 256; i++) {
        *pSpectrum++ = i;
    }
    
    m_memory.dsp_xy[1].xchans = 256;
    m_memory.dsp_xy[1].ychans = 256;
    m_memory.dsp_offsets[1]   = 256;    // units of longwords.
    m_memory.dsp_types[1] = twodlong;   // For long word spectra.
    
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++)
        *pSpectrum++ = i+j;
    }
    
    
    // The usual 2two updates and validations:
    //
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the full update header and be sure it's what's expected.
    
    int s = client.Read(&update, sizeof(update));
    
    size_t expectedSpectrumSize = (256*256 + 256)*sizeof(uint32_t);
    size_t expectedHeaderSize   = offsetof(Xamine_shared, dsp_spectra);
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    EQ(
       sizeof(update) + expectedSpectrumSize + expectedHeaderSize,
       size_t(update.s_messageSize)
    );
    // Read/validate the data:
    
    static Xamine_shared mirror;   // So as not to run into stack limits.
    s = client.Read(&mirror, expectedSpectrumSize + expectedHeaderSize);
    EQ(expectedSpectrumSize + expectedHeaderSize, size_t(s));
    EQ(m_memory.dsp_xy[0].xchans, mirror.dsp_xy[0].xchans);
    EQ(m_memory.dsp_xy[0].ychans, mirror.dsp_xy[0].ychans);
    EQ(m_memory.dsp_offsets[0], mirror.dsp_offsets[0]);
    EQ(m_memory.dsp_types[0], mirror.dsp_types[0]);
    
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
    
    
    // The next should just read the spectra.
    
    memset(&mirror, 0, sizeof(mirror));
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Should be a partial update with expectedSpectrumSize payload:
    
    s = client.Read(&update, sizeof(update));
    EQ(Mirror::MSG_TYPE_PARTIAL_UPDATE, update.s_messageType);
    EQ(expectedSpectrumSize + sizeof(update), size_t(update.s_messageSize));
    
    s = client.Read(&mirror.dsp_spectra.XAMINE_l, expectedSpectrumSize);
    EQ(expectedSpectrumSize, size_t(s));
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
       
    client.Shutdown();
    
}

// Two spectra with a hole .. note that the hole is also transferred.

void
mservertest::spectrum_4()
{
    m_memory.dsp_xy[0].xchans = 256;
    m_memory.dsp_xy[0].ychans = 0;
    m_memory.dsp_offsets[0] = 0;
    m_memory.dsp_types[0] = onedlong;
    
    uint32_t* pSpectrum = m_memory.dsp_spectra.XAMINE_l;
    for (int i = 0; i < 256; i++) {
        *pSpectrum++ = i;
    }
    pSpectrum += 100;               //  hole of 100 longwords.
    
    m_memory.dsp_xy[1].xchans = 256;
    m_memory.dsp_xy[1].ychans = 256;
    m_memory.dsp_offsets[1]   = 256+100;    // units of longwords.
    m_memory.dsp_types[1] = twodlong;   // For long word spectra.
    
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++)
        *pSpectrum++ = i+j;
    }
    
    // The usual 2two updates and validations:
    //
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the full update header and be sure it's what's expected.
    
    int s = client.Read(&update, sizeof(update));
    
    size_t expectedSpectrumSize = (256*256 + 256 + 100)*sizeof(uint32_t);
    size_t expectedHeaderSize   = offsetof(Xamine_shared, dsp_spectra);
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    EQ(
       sizeof(update) + expectedSpectrumSize + expectedHeaderSize,
       size_t(update.s_messageSize)
    );
    // Read/validate the data:
    
    static Xamine_shared mirror;   // So as not to run into stack limits.
    s = client.Read(&mirror, expectedSpectrumSize + expectedHeaderSize);
    EQ(expectedSpectrumSize + expectedHeaderSize, size_t(s));
    EQ(m_memory.dsp_xy[0].xchans, mirror.dsp_xy[0].xchans);
    EQ(m_memory.dsp_xy[0].ychans, mirror.dsp_xy[0].ychans);
    EQ(m_memory.dsp_offsets[0], mirror.dsp_offsets[0]);
    EQ(m_memory.dsp_types[0], mirror.dsp_types[0]);
    
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
    
    
    // The next should just read the spectra.
    
    memset(&mirror, 0, sizeof(mirror));
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Should be a partial update with expectedSpectrumSize payload:
    
    s = client.Read(&update, sizeof(update));
    EQ(Mirror::MSG_TYPE_PARTIAL_UPDATE, update.s_messageType);
    EQ(expectedSpectrumSize + sizeof(update), size_t(update.s_messageSize));
    
    s = client.Read(&mirror.dsp_spectra.XAMINE_l, expectedSpectrumSize);
    EQ(expectedSpectrumSize, size_t(s));
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
       
    client.Shutdown();
}

//Test sizing of onedword

void
mservertest::spectrum_5()
{
    m_memory.dsp_xy[0].xchans = 256;
    m_memory.dsp_xy[0].ychans = 0;
    m_memory.dsp_offsets[0] = 0;
    m_memory.dsp_types[0] = onedword;
    
    uint16_t* pSpectrum = m_memory.dsp_spectra.XAMINE_w;
    for (int i = 0; i < 256; i++) {
        *pSpectrum++ = i;
    }
    // The usual 2two updates and validations:
    //
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the full update header and be sure it's what's expected.
    
    int s = client.Read(&update, sizeof(update));
    
    size_t expectedSpectrumSize = (256)*sizeof(uint16_t);
    size_t expectedHeaderSize   = offsetof(Xamine_shared, dsp_spectra);
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    EQ(
       sizeof(update) + expectedSpectrumSize + expectedHeaderSize,
       size_t(update.s_messageSize)
    );
    // Read/validate the data:
    
    static Xamine_shared mirror;   // So as not to run into stack limits.
    s = client.Read(&mirror, expectedSpectrumSize + expectedHeaderSize);
    EQ(expectedSpectrumSize + expectedHeaderSize, size_t(s));
    EQ(m_memory.dsp_xy[0].xchans, mirror.dsp_xy[0].xchans);
    EQ(m_memory.dsp_xy[0].ychans, mirror.dsp_xy[0].ychans);
    EQ(m_memory.dsp_offsets[0], mirror.dsp_offsets[0]);
    EQ(m_memory.dsp_types[0], mirror.dsp_types[0]);
    
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
    
    
    // The next should just read the spectra.
    
    memset(&mirror, 0, sizeof(mirror));
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Should be a partial update with expectedSpectrumSize payload:
    
    s = client.Read(&update, sizeof(update));
    EQ(Mirror::MSG_TYPE_PARTIAL_UPDATE, update.s_messageType);
    EQ(expectedSpectrumSize + sizeof(update), size_t(update.s_messageSize));
    
    s = client.Read(&mirror.dsp_spectra.XAMINE_l, expectedSpectrumSize);
    EQ(expectedSpectrumSize, size_t(s));
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
       
    client.Shutdown();
}
// test sizing of twodword
void
mservertest::spectrum_6()
{
    m_memory.dsp_xy[1].xchans = 256;
    m_memory.dsp_xy[1].ychans = 256;
    m_memory.dsp_offsets[1]   = 0;    // units of longwords.
    m_memory.dsp_types[1] = twodword;   // For long word spectra.
    
    uint16_t* pSpectrum = m_memory.dsp_spectra.XAMINE_w;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++)
        *pSpectrum++ = i+j;
    }
    // The usual 2two updates and validations:
    //
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the full update header and be sure it's what's expected.
    
    int s = client.Read(&update, sizeof(update));
    
    size_t expectedSpectrumSize = (256*256)*sizeof(uint16_t);
    size_t expectedHeaderSize   = offsetof(Xamine_shared, dsp_spectra);
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    EQ(
       sizeof(update) + expectedSpectrumSize + expectedHeaderSize,
       size_t(update.s_messageSize)
    );
    // Read/validate the data:
    
    static Xamine_shared mirror;   // So as not to run into stack limits.
    s = client.Read(&mirror, expectedSpectrumSize + expectedHeaderSize);
    EQ(expectedSpectrumSize + expectedHeaderSize, size_t(s));
    EQ(m_memory.dsp_xy[0].xchans, mirror.dsp_xy[0].xchans);
    EQ(m_memory.dsp_xy[0].ychans, mirror.dsp_xy[0].ychans);
    EQ(m_memory.dsp_offsets[0], mirror.dsp_offsets[0]);
    EQ(m_memory.dsp_types[0], mirror.dsp_types[0]);
    
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
    
    
    // The next should just read the spectra.
    
    memset(&mirror, 0, sizeof(mirror));
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Should be a partial update with expectedSpectrumSize payload:
    
    s = client.Read(&update, sizeof(update));
    EQ(Mirror::MSG_TYPE_PARTIAL_UPDATE, update.s_messageType);
    EQ(expectedSpectrumSize + sizeof(update), size_t(update.s_messageSize));
    
    s = client.Read(&mirror.dsp_spectra.XAMINE_l, expectedSpectrumSize);
    EQ(expectedSpectrumSize, size_t(s));
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
       
    client.Shutdown();
    
}
// Test sizing of twodbyte

void
mservertest::spectrum_7()
{
m_memory.dsp_xy[1].xchans = 256;
    m_memory.dsp_xy[1].ychans = 256;
    m_memory.dsp_offsets[1]   = 0;    // units of longwords.
    m_memory.dsp_types[1] = twodbyte;   // For long word spectra.
    
    uint8_t* pSpectrum = m_memory.dsp_spectra.XAMINE_b;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++)
        *pSpectrum++ = i+j;
    }
    // The usual 2two updates and validations:
    //
    CSocket client;
    std::string sport = std::to_string(currentPort);
    CPPUNIT_ASSERT_NO_THROW(client.Connect("localhost", sport.c_str()));

    
    struct _memory {
        Mirror::MessageHeader  s_header;
        Mirror::MemoryKey      s_key;
    } message;
    
    message.s_header.s_messageSize = sizeof(message);
    message.s_header.s_messageType = Mirror::MSG_TYPE_SHMINFO;
    memcpy(message.s_key, "ABCD", 4);
    
    client.Write(&message, sizeof(message));
    client.Flush();
    
    // Now ask for an update:
    
    Mirror::MessageHeader update;
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Read the full update header and be sure it's what's expected.
    
    int s = client.Read(&update, sizeof(update));
    
    size_t expectedSpectrumSize = (256*256)*sizeof(uint8_t);
    size_t expectedHeaderSize   = offsetof(Xamine_shared, dsp_spectra);
    EQ(Mirror::MSG_TYPE_FULL_UPDATE, update.s_messageType);
    EQ(
       sizeof(update) + expectedSpectrumSize + expectedHeaderSize,
       size_t(update.s_messageSize)
    );
    // Read/validate the data:
    
    static Xamine_shared mirror;   // So as not to run into stack limits.
    s = client.Read(&mirror, expectedSpectrumSize + expectedHeaderSize);
    EQ(expectedSpectrumSize + expectedHeaderSize, size_t(s));
    EQ(m_memory.dsp_xy[0].xchans, mirror.dsp_xy[0].xchans);
    EQ(m_memory.dsp_xy[0].ychans, mirror.dsp_xy[0].ychans);
    EQ(m_memory.dsp_offsets[0], mirror.dsp_offsets[0]);
    EQ(m_memory.dsp_types[0], mirror.dsp_types[0]);
    
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
    
    
    // The next should just read the spectra.
    
    memset(&mirror, 0, sizeof(mirror));
    update.s_messageSize = sizeof(Mirror::MessageHeader);
    update.s_messageType = Mirror::MSG_TYPE_REQUEST_UPDATE;
    
    client.Write(&update, sizeof(update));
    client.Flush();
    
    // Should be a partial update with expectedSpectrumSize payload:
    
    s = client.Read(&update, sizeof(update));
    EQ(Mirror::MSG_TYPE_PARTIAL_UPDATE, update.s_messageType);
    EQ(expectedSpectrumSize + sizeof(update), size_t(update.s_messageSize));
    
    s = client.Read(&mirror.dsp_spectra.XAMINE_l, expectedSpectrumSize);
    EQ(expectedSpectrumSize, size_t(s));
    EQ(
       0,
       memcmp(
            m_memory.dsp_spectra.XAMINE_b, mirror.dsp_spectra.XAMINE_b,
            expectedSpectrumSize
        )
    );
       
    client.Shutdown();    
}
