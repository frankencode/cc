/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/System>
#include <cc/Date>

using namespace cc;
using namespace cc::testing;

class BasicDateFormatting: public TestCase
{
    void run()
    {
        double t = System::now();
        fout() << t << nl;
        Date d1{Date{t}->time()};
        Date d2{t};

        fout() << d1->dayName() << ", " << d1 << nl;
        fout() << d2->dayName() << ", " << d2 << nl;

        CC_VERIFY(str(d1) == str(d2));
    }
};

class BeginOfEpoch: public TestCase
{
    void run()
    {
        Date date{0};
        fout() << date << nl;
        fout() << "year: " << date->year() << nl;
        fout() << "month: " << date->month() << nl;
        fout() << "day: " << date->day() << nl;

        CC_VERIFY(date->isValid());
        CC_VERIFY(date->year() == 1970);
        CC_VERIFY(date->month() == 1);
        CC_VERIFY(date->day() == 1);
    }
};

class CalendarDates: public TestCase
{
    void run()
    {
        struct TestDate { int day; int month; int year; int hour; int minutes; int seconds; };
        TestDate tests[] = {
            {  1,  1, 2250,  0,  0,  0 },
            { 12,  9, 2013,  0,  0,  0 },
            {  3,  9, 2012, 11, 22, 33 },
            {  1,  7, 1981,  0,  0,  0 },
            {  1,  1, 1970,  0,  0,  0 },
            { 24,  7, 1969, 16, 50, 35 },
            {  1,  1, 1968,  0,  0,  0 },
            {  1,  1, 1967,  0,  0,  0 },
            {  1,  7, 1967,  0,  0,  0 },
            {  1,  1, 1900,  0,  0,  0 },
            {  1,  1,    1,  0,  0,  0 }
        };
        for (unsigned i = 0; i < sizeof(tests)/sizeof(tests[0]); ++i) {
            TestDate *d = &tests[i];
            Date date0{d->year, d->month, d->day, d->hour, d->minutes, d->seconds};
            Date date1{date0->time()};
            fout() << int64_t(date0->time()) << nl;
            fout() << "  " << date0->dayName() << ", " << date0 << " == " << date1->dayName() << ", " << date1 << nl;
            CC_VERIFY(date0->time() == date1->time());
            CC_VERIFY(date0->toString() == date1->toString());
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(BasicDateFormatting);
    CC_TESTSUITE_ADD(BeginOfEpoch);
    CC_TESTSUITE_ADD(CalendarDates);

    return TestSuite::instance()->run(argc, argv);
}

#if 0
namespace cc {
    bool leapYear(int y);
}

if (false) {
    fout() << "Compute linear day number of 1st Jan 1970..." << nl;
    int n = 0;
    for (int y = 1969; y >= 0; --y) n += 365 + leapYear(y);
    fout() << n << nl;
}
if (false) {
    double t = Date::create(9999, 12, 31)->time();
    while (true) {
        Ref<Date> d1 = Date::create(t);
        Ref<Date> d2 = Date::create(d1->year(), d1->month() + 1, d1->day());
        fout() << "t = " << int64_t(t) << "  " << d1 << nl;
        if (d1->year() != d2->year() || d1->month() != d2->month() || d1->day() != d2->day()) {
            fout() << d1 << " != " << d2 << nl;
            CC_VERIFY(false);
        }
        if (d1->time() != d2->time()) {
            fout() << d1 << " != " << d2 << nl;
            fout() << "  " << int64_t(d1->time()) << " != " << int64_t(d2->time()) << nl;
            fout() << "  " << int64_t(d1->time()) - int64_t(d2->time()) << nl;
            CC_VERIFY(false);
        }
        if (d1->year() == 1 && d1->month() == 1 && d1->day() == 1) break;
        t -= 86400;
    }
}
#endif
