#ifndef MYPARAMETERMAPPER_H
#define MYPARAMETERMAPPER_H

#include <ParameterMapper.h>

#include <map>
#include <utility>

class CEvent;       // SpecTcl event, we don't use it.
class MyParameters; // The strucutre of tree parameters.

/**
 * @struct ModuleInfo
 * @brief Contains information about a DDAS module.
 */
struct ModuleInfo {
  int s_crateId;   //!< Crate ID of module.
  int s_slotId;    //!< Slot ID of module.
  int s_nChannels; //!< Number of channels in module.

  /**
   * @brief Compare two ModuleInfo instances.
   * @param other The other module info.
   * @return True if this module info comes before the other based on crate and
   * slot ID, false otherwise.
   */
  bool operator<(const ModuleInfo &other) const {
    return std::make_pair(s_crateId, s_slotId) <
           std::make_pair(other.s_crateId, other.s_slotId);
  }
};

/**
 * @class MyParameterMapper
 * @brief This class is responsible for mapping DDAS hit data to SpecTcl
 * TreeParameters.
 * @details
 * This class operates on a MyParameters structure containing TreeParameters but
 * does not own it. The constructor takes a reference to the MyParameters
 * structure and initializes a channel map based on the provided module
 * information. The mapToParameters function takes raw hit data and fills the
 * appropriate TreeParameters.
 */
class MyParameterMapper : public DAQ::DDAS::CParameterMapper {
private:
  MyParameters &m_params; //<! Reference to the tree parameter structure.
  std::map<std::pair<int, int>, int>
      m_chanMap; //<! Map of crate and slot IDs to base channel index.

public:
  /**
   * @brief Constructor.
   * @param params The data structure.
   */
  MyParameterMapper(MyParameters &params);

  /**
   * @brief Map raw hit data to tree parameters.
   * @param channelData The hit data.
   * @param rEvent The SpecTcl event.
   */
  virtual void mapToParameters(const std::vector<ddasfmt::DDASHit> &channelData,
                               CEvent &rEvent);

private:
  /**
   * @brief Build the channel map from module information.
   * @param modules The list of module information.
   */
  void buildChannelMap(std::vector<ModuleInfo> &modules);
  /**
   * @brief Compute channel index from crate, slot, and channel information.
   * @param hit The hit data.
   * @return The global channel index.
   * @throws std::out_of_range if the crate and slot combination is not found in
   * the channel map.
   */
  int computeGlobalIndex(const ddasfmt::DDASHit &hit);
};

#endif
