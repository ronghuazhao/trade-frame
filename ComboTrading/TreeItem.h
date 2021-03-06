/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

// started December 13, 2015, 2:13 PM

#pragma once

#include <boost/signals2.hpp>

//#include <wx/panel.h>
#include <wx/window.h>

#include <TFBitsNPieces/TreeOpsItems.h>

#include "InstrumentInfo.h"

struct Resources {

  //wxWindow* m_pWin;  // is this used somewhere?  maybe is the charting window, which is now in InstrumentInfo
  
  //typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef InstrumentInfo::pInstrumentInfo_t pInstrumentInfo_t;
  
  enum ENewInstrumentLock { NoLock, LockOption, LockFuturesOption };
  
  // used in TreeItemInstrument
  typedef boost::signals2::signal<pInstrumentInfo_t (ENewInstrumentLock), ou::tf::FirstOrDefault<pInstrumentInfo_t> > signalNewInstrumentInfo_t;
  typedef signalNewInstrumentInfo_t::slot_type slotNewInstrumentInfo_t;
  signalNewInstrumentInfo_t signalNewInstrumentViaDialog;
  
  typedef boost::signals2::signal<pInstrumentInfo_t (const std::string&), ou::tf::FirstOrDefault<pInstrumentInfo_t> > signalLoadInstrumentInfo_t;
  typedef signalLoadInstrumentInfo_t::slot_type slotLoadInstrumentInfo_t;
  signalLoadInstrumentInfo_t signalLoadInstrument;
  
  //Resources( void ): m_pWin( 0 ) {}
};

// ================

class TreeItemResources: public ou::tf::TreeItemBase {
  friend class boost::serialization::access;
public:
  TreeItemResources( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
    ou::tf::TreeItemBase( id, baseResources ), m_resources( resources ) {}
  virtual ~TreeItemResources( void ) {};
protected:
  Resources& m_resources;
private:
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const ou::tf::TreeItemBase>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<ou::tf::TreeItemBase>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
