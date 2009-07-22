#include "SignalSet.hpp"

namespace pona
{

SignalSet::SignalSet() { clear(); }
void SignalSet::clear() { sigemptyset(&rawSet_); }
void SignalSet::fill() { sigfillset(&rawSet_); }
void SignalSet::add(int signal) { sigaddset(&rawSet_, signal); }
void SignalSet::del(int signal) { sigdelset(&rawSet_, signal); }
bool SignalSet::contains(int signal) const { return sigismember(&rawSet_, signal); }
sigset_t* SignalSet::rawSet() { return &rawSet_; }

} // namespace pona
