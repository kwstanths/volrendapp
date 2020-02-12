#ifndef __Bezier_hpp__
#define __Bezier_hpp__

#include <qgraphicsitem.h>
#include <qpainter.h>

class Node;

class Bezier : public QGraphicsItem
{

public:
	virtual void adjust() = 0;
	
};

#endif
