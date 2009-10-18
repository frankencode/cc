#ifndef PONA_SIGNALSET_HPP
#define PONA_SIGNALSET_HPP

#include <signal.h>
#include "atoms"

namespace pona
{

class PONA_API SignalSet: public Instance
{
public:
	SignalSet();
	
	void clear();
	void fill();
	void add(int signal);
	void del(int signal);
	
	bool contains(int signal) const;
	
	sigset_t* rawSet();
	
private:
	sigset_t rawSet_;
};

} // namespace pona

#endif // PONA_SIGNALSET_HPP
