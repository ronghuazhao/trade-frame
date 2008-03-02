#include "StdAfx.h"
#include "DataManager.h"
#include <stdexcept>

using namespace H5;

// H5FD_FAMILY
/*

    herr_t H5Pset_fapl_family (hid_t fapl_id, hsize_t memb_size,
             hid_t member_properties) 
    herr_t H5Pget_fapl_family (hid_t fapl_id, hsize_t *memb_size,
             hid_t *member_properties) 
*/

const char CDataManager::m_H5FileName[] = "TradeFrame.%03d.hdf5";
H5File CDataManager::m_H5File;
DbEnv CDataManager::m_DbEnv( 0 );
unsigned int CDataManager::m_RefCount = 0;

CDataManager dm;

CDataManager::CDataManager(void) {
  ++m_RefCount;
  if ( 1 == m_RefCount ) {
    m_DbEnv.open( ".", DB_INIT_MPOOL | DB_CREATE | DB_THREAD | DB_INIT_CDB, 0 );
    H5::FileAccPropList pl1;
    H5::FileAccPropList pl2;
    pl2.setFamily( 2048000000, pl1 );
    try {
      try {
        // try for existing file
        m_H5File.openFile( m_H5FileName, H5F_ACC_RDWR, pl2 );
      }
      catch (...) {
        // try to create and open
        m_H5File.openFile( m_H5FileName, H5F_ACC_CREAT | H5F_ACC_RDWR, pl2 );
        Group g1( dm.GetH5File()->createGroup( "/daily" ) );
        g1.close();
        Group g2( dm.GetH5File()->createGroup( "/daily/bar" ) );
        g2.close();
      }

    }
    catch (...) {
      printf( "problems with HDF5 system\n" );
    }
  }
  else {
    //throw std::exception( "Only one DataManager may be instantiated." );
    // multiple instantiations are allowed
  }
}

CDataManager::~CDataManager(void) {
  --m_RefCount;
  if ( 0 == m_RefCount ) {
    m_H5File.close();
    m_DbEnv.close( 0 );
  }
}
