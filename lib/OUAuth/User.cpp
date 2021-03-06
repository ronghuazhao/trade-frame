/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// started 2013-05-18

#include "User.h"

namespace ou { // One Unified
namespace auth { // authentication

User::User(const std::string& sLogin, const std::string& sPassword,
  const std::string& sFirstName, const std::string& sLastName, const std::string& sEmail ) 
  : m_row( false, sLogin, sPassword, sFirstName, sLastName, sEmail )
{
}

User::~User(void) {
}

} // namespace auth
} // namespace ou
