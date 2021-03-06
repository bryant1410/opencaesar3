// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "forum.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "walker/taxcollector.hpp"
#include "game/city.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"

class Forum::Impl
{
public:
  int taxInThisMonth;
  int citizensReached;
};

Forum::Forum() : ServiceBuilding(Service::forum, constants::building::forum, Size(2)), _d( new Impl )
{
  _d->taxInThisMonth = 0;
  _d->citizensReached = 0;
  setPicture( ResourceGroup::govt, 10 );
}

void Forum::deliverService()
{
  if( getWorkers() > 0 && getWalkerList().size() == 0 )
  {
    TaxCollectorPtr walker = TaxCollector::create( _getCity() );
    walker->send2City( this );

    if( !walker->isDeleted() )
    {
      addWalker( walker.as<Walker>() );
    }
  }
}

void Forum::timeStep( const unsigned long time )
{
  WalkerList walkers = getWalkerList();
  foreach( WalkerPtr walker, walkers )
  {
    if( walker->isDeleted() )
    {
      TaxCollectorPtr collector = walker.as< TaxCollector >();
      if( collector.isValid() )
      {
        _d->taxInThisMonth += collector->getMoney();
        _d->citizensReached += collector->getCitizensReachedCount();
      }
    }
  }

  Building::timeStep( time );
}

int Forum::collectTaxes()
{
  int taxes = _d->taxInThisMonth;
  _d->taxInThisMonth = 0;
  _d->citizensReached = 0;
  return taxes;
}

int Forum::getPeoplesReached() const
{
  return _d->citizensReached;
}
