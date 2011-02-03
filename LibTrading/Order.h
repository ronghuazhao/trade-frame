/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

// should provider be included?  No, this allows an order routing process to select an 
//   an appropriate provider based upon other criteria
// the provider will be associated later for Execution evaluation

#pragma once

#include <string>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;
#include <boost/cstdint.hpp>

#include <LibCommon/Delegate.h>

#include "TradingEnumerations.h"
#include "Instrument.h"
#include "Execution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class COrderManager;

class COrder {
  friend class COrderManager;
public:

  typedef boost::int64_t idOrder_t;
  typedef boost::int64_t idPosition_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::idInstrument_t idInstrument_t;
  typedef boost::shared_ptr<COrder> pOrder_t;
  typedef const pOrder_t& pOrder_ref;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "orderid", idOrder );
      ou::db::Field( a, "positionid", idPosition );
      ou::db::Field( a, "instrumentid", idInstrument );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "orderstatus", eOrderStatus );
      ou::db::Field( a, "ordertype", eOrderType );
      ou::db::Field( a, "orderside", eOrderSide );
      ou::db::Field( a, "price1", dblPrice1 );
      ou::db::Field( a, "price2", dblPrice2 );
      ou::db::Field( a, "signalprice", dblSignalPrice );
      ou::db::Field( a, "quantityordered", nOrderQuantity );
      ou::db::Field( a, "quantityremaining", nQuantityRemaining );
      ou::db::Field( a, "quantityfilled", nQuantityFilled );
      ou::db::Field( a, "averagefillprice", dblAverageFillPrice );
      ou::db::Field( a, "commission", dblCommission );
      ou::db::Field( a, "datetimecreated", dtOrderCreated );
      ou::db::Field( a, "datetimesubmitted", dtOrderSubmitted );
      ou::db::Field( a, "datetimeclosed", dtOrderClosed );

      ou::db::Key( a, "orderid" );
      ou::db::Constraint( a, "positionid", "positions", "positionid" );
      ou::db::Constraint( a, "instrumentid", "instruments", "instrumentid" );
    }
    //"create index idx_orders_positionid on orders( positionid );",

    idOrder_t idOrder;
    idPosition_t idPosition;
    idInstrument_t idInstrument;
    std::string sDescription;
    OrderStatus::enumOrderStatus eOrderStatus;
    OrderType::enumOrderType eOrderType;
    OrderSide::enumOrderSide eOrderSide;
    double dblPrice1; // for limit
    double dblPrice2; // for stop
    double dblSignalPrice;  // mark at which algorithm requested order
    unsigned long nOrderQuantity;
    unsigned long nQuantityRemaining;
    unsigned long nQuantityFilled;
    double dblAverageFillPrice;  // excludes commission
    double dblCommission;
    ptime dtOrderCreated;
    ptime dtOrderSubmitted;
    ptime dtOrderClosed;

  };

  const static std::string m_sTableName;

  COrder(  // market 
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit or stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,  
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit and stop
    CInstrument::pInstrument_cref instrument, 
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,  
    double dblPrice2,
    idPosition_t idPosition = 0,
    ptime dtOrderSubmitted = not_a_date_time
    );
  ~COrder(void);

  void SetOutsideRTH( bool bOutsideRTH ) { m_bOutsideRTH = bOutsideRTH; };  // not persisted yet
  bool GetOutsideRTH( void ) const { return m_bOutsideRTH; };
  void SetInstrument( CInstrument::pInstrument_cref pInstrument ) {  // used only when class created from database
    if ( NULL != m_pInstrument.get() ) {
      throw std::runtime_error( "Corder::SetInstrument: instrument already assigned" );
    }
    if ( m_idInstrument != pInstrument->GetInstrumentName() ) {
      throw std::runtime_error( "COrder::SetInstrument: instrument name does not match expected" );
    }
    m_pInstrument = pInstrument;
  }
  CInstrument::pInstrument_t GetInstrument( void ) const { 
    if ( NULL == m_pInstrument.get() ) {
      throw std::runtime_error( "COrder::GetInstrument:  no instrument defined" );
    }
    return m_pInstrument; 
  };
  const char *GetOrderSideName( void ) const { return OrderSide::Name[ m_row.eOrderSide ]; };
  unsigned long GetQuantity( void ) const { return m_row.nOrderQuantity; };
  OrderType::enumOrderType GetOrderType( void ) const { return m_row.eOrderType; };
  OrderSide::enumOrderSide GetOrderSide( void ) const { return m_row.eOrderSide; };
  double GetPrice1( void ) const { return m_row.dblPrice1; };  // need to validate this on creation
  double GetPrice2( void ) const { return m_row.dblPrice2; };
  double GetAverageFillPrice( void ) const { return m_row.dblAverageFillPrice; };
  idOrder_t GetOrderId( void ) const { assert( 0 != m_row.idOrder ); return m_row.idOrder; };
  unsigned long GetNextExecutionId( void ) { return ++m_nNextExecutionId; };
  void SetSendingToProvider( void );
  OrderStatus::enumOrderStatus ReportExecution( const CExecution &exec ); // called from COrderManager
  void SetCommission( double dblCommission );
  double GetCommission( void ) const{ return m_row.dblCommission; };
  void ActOnError( OrderErrors::enumOrderErrors eError );
  unsigned long GetQuanRemaining( void ) const { return m_row.nQuantityRemaining; };
  unsigned long GetQuanOrdered( void ) const { return m_row.nOrderQuantity; };
  unsigned long GetQuanFilled( void ) const { return m_row.nQuantityFilled; };
  void SetSignalPrice( double dblSignalPrice ) { m_row.dblSignalPrice = dblSignalPrice; };
  double GetSignalPrice( void ) const { return m_row.dblSignalPrice; };
  const ptime &GetDateTimeOrderSubmitted( void ) const { 
    assert( not_a_date_time != m_row.dtOrderSubmitted ); // is this a valid test?
    return m_row.dtOrderSubmitted; 
  };
  const ptime &GetDateTimeOrderFilled( void ) const { 
    assert( not_a_date_time != m_row.dtOrderClosed ); // is this a valid test?
    return m_row.dtOrderClosed; 
  };

  ou::Delegate<const std::pair<const COrder&, const CExecution&>& > OnExecution;
  ou::Delegate<const COrder&> OnOrderFilled; // on final fill
  ou::Delegate<const COrder&> OnPartialFill; // on intermediate fills only
  ou::Delegate<const COrder&> OnCommission;

protected:

  CInstrument::pInstrument_t m_pInstrument;
  idInstrument_t m_idInstrument;  // used temporarily in order to get instrument_t in place

  bool m_bOutsideRTH;

  unsigned long m_nNextExecutionId;

  // statistics and status
  double m_dblPriceXQuantity; // used for calculating average price

  void ConstructOrder( void );
  void SetOrderId( idOrder_t );  // used by OrderManager

private:

  TableRowDef m_row;

  COrder(void);  // no default constructor

};

} // namespace tf
} // namespace ou
