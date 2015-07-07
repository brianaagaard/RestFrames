/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2015, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFKey.hh
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2015 Jul
///
//   This file is part of RestFrames.
//
//   RestFrames is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   RestFrames is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with RestFrames. If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////

#ifndef RFKey_HH
#define RFKey_HH

namespace RestFrames {

  ///////////////////////////////////////////////
  // RFKey class
  ///////////////////////////////////////////////
  class RFKey {

  public:
    RFKey();
    RFKey(int key);
    RFKey(const RFKey& key);
    ~RFKey();

    void operator=(const RFKey& key);
    bool operator==(const RFKey& key) const;
    void SetKey(int key);
    int GetKey() const;
    bool IsSame(const RFKey& key) const;

  private:
    int m_Key;

  };

  extern RFKey g_Key;

}

#endif
