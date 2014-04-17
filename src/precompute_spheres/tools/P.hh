#ifndef P_HH
#define P_HH

#include "Geometry.h"

struct P : public gk::Point
{
	P()                   : gk::Point()  {}
	P(const gk::Point& p) : gk::Point(p) {}
};
struct PN : public gk::Point
{
	PN()                                        : gk::Point(),  normal()  {}
	PN(const gk::Point& p)                      : gk::Point(p), normal()  {}
	PN(const gk::Point& p, const gk::Normal& n) : gk::Point(p), normal(n) {}
	//------------------------------------------------
	gk::Normal normal;
};

#endif