//
// FILE: nfg.cc -- Implementation of normal form member functions
//              -- and Implementation of NFPlayer member functions
//
// $Id$
//

#include <assert.h>
#include "nfg.h"
#include "nfstrat.h"
#include "nfplayer.h"


// ---------------------------------------------
// BaseNfg: Constructors, Destructors, Operators
// ---------------------------------------------


int BaseNfg::Product(const gArray<int> &dim)
{
  int accum = 1;
  for (int i = 1; i <= dim.Length(); accum *= dim[i++]);
  return accum;
}
  
BaseNfg::BaseNfg(const NfgPayoffs &pay, const gArray<int> &dim)
  : dimensions(dim), players(dim.Length()), results(Product(dim)),
    paytable((NfgPayoffs *) &pay)
{
  for (int pl = 1; pl <= players.Length(); pl++)  {
    players[pl] = new NFPlayer(pl, this, dim[pl]);
    players[pl]->name = ToString(pl);
    for (int st = 1; st <= players[pl]->NumStrats(); st++)
      players[pl]->strategies[st]->name = ToString(st);
  }
  IndexStrategies();

  outcomes.Append(new NFOutcome(1, this));
  for (int cont = 1; cont <= results.Length();
       results[cont++] = outcomes[1]);
}

BaseNfg::BaseNfg (const BaseNfg &b)
  : title(b.title), dimensions(b.dimensions),
    players(b.players.Length()), paytable(b.paytable)
{
  for (int i = 1; i <= players.Length(); i++){
    players[i] = new NFPlayer(i, this, dimensions[i]);
    players[i]->name = b.players[i]->name;
    for (int j = 1; j <= players[i]->NumStrats(); j++)  {
      *(players[i]->strategies[j]) = *(b.players[i]->strategies[j]);
      players[i]->strategies[j]->nfp = players[i];
    }
  }
  IndexStrategies();
}

#include "efg.h"

BaseNfg::~BaseNfg()
{
  for (int i = 1; i <= players.Length(); i++)
    delete players[i];
}


//-------------------------
//BaseNfg: Member Functions
//-------------------------


NFOutcome *BaseNfg::NewOutcome(void)
{
  NFOutcome *outcome = new NFOutcome(outcomes.Length() + 1, this);
  outcomes.Append(outcome);

  return outcome;
}

const gArray<Strategy *> &BaseNfg::Strategies(int p) const
{
  return (players[p]->Strategies());
}

void BaseNfg::SetTitle(const gString &s) 
{ 
  title = s; 
}

const gString &BaseNfg::GetTitle(void) const 
{ 
  return title; 
}

int BaseNfg::NumPlayers(void) const 
{ 
  return (players.Length()); 
}

const gArray<NFPlayer *> &BaseNfg::Players(void) const
{
  return players;
}

int BaseNfg::NumStrats(int pl) const
{
  return ((pl > 0 && pl <= NumPlayers()) ? 
	  (players[pl])->strategies.Length() : 0);
}

int BaseNfg::ProfileLength(void) const
{
  int nprof = 0;
  for (int i = 1; i <= players.Length(); i++)
    nprof += players[i]->strategies.Length();
  return nprof;
}

//
// Note that the link with the associated efg, if any, is broken
// only on this end, and the Lexicon of the efg is not deleted
// This shouldn't be a problem, so the code has not been changed 
// to delete the Lexicon immediately
//
void BaseNfg::SetOutcome(const gArray<int> &profile, NFOutcome *outcome)
{
  int index = 1;
  for (int i = 1; i <= profile.Length(); i++)
    index += players[i]->strategies[profile[i]]->index;
  results[index] = outcome;
//  BreakLink();
}


void BaseNfg::SetOutcome(const StrategyProfile &p, NFOutcome *outcome)
{
  results[p.index + 1] = outcome;
//  BreakLink();
}

NFOutcome *BaseNfg::GetOutcome(const gArray<int> &profile) const 
{
  int index = 1;
  for (int i = 1; i <= profile.Length(); i++)
    index += players[i]->strategies[profile[i]]->index;
  return results[index];
}

NFOutcome *BaseNfg::GetOutcome(const StrategyProfile &p) const
{
  return results[p.index + 1];
}


// ---------------------------------------
// BaseNfg: Private member functions
// ---------------------------------------

void BaseNfg::IndexStrategies(void)
{
  long offset = 1L;

  for (int i = 1; i <= NumPlayers(); i++)  {
    int j;
    for (j = 1; j <= NumStrats(i); j++)  {
      Strategy *s = (players[i])->strategies[j];
      s->number = j;
      s->index = (j - 1) * offset;
    }
    assert(j - 1 == NumStrats(i));
    offset *= (j - 1);
  }
}

// --------------------------
// NFPlayer: Member functions 
// --------------------------

NFPlayer::NFPlayer(int n, BaseNfg *N, int num)
: number(n), N(N), strategies(num)
{ 
  for (int j = 1; j <= num; j++)
    strategies[j] = new Strategy(this);
}

NFPlayer::~NFPlayer()
{ 
  for (int j = 1; j <= strategies.Length(); j++)
    delete strategies[j];
}

BaseNfg &NFPlayer::BelongsTo(void) const
{
  return *N;
}

const gString &NFPlayer::GetName(void) const
{
  return name;
}

void NFPlayer::SetName(const gString &s)
{
  name = s;
}

int NFPlayer::NumStrats(void) const 
{
  return strategies.Length();
}


const gArray<Strategy *> &NFPlayer::Strategies(void) const
{
  return strategies;
}



// Not really how these should be handled; but it works for now, and
// we can deal with housekeeping later

#include "nfgiter.imp"
#include "nfgciter.imp"


#ifdef __GNUG__
#define TEMPLATE template
#elif defined __BORLANDC__
#define TEMPLATE
#pragma option -Jgd
#endif   // __GNUG__, __BORLANDC__

#include "garray.imp"
#include "gblock.imp"
#include "glist.imp"

TEMPLATE class gArray<NFStrategySet *>;
TEMPLATE class gArray<Strategy *>;
TEMPLATE class gArray<NFOutcome *>;

TEMPLATE class gArray<NFPlayer *>;
TEMPLATE class gBlock<Strategy *>;

TEMPLATE class gList<NFOutcome *>;
TEMPLATE class gNode<NFOutcome *>;
