#include "BaseStrokeHandler.h"

#include "gui/XournalView.h"
#include "control/Control.h"
#include "control/layer/LayerController.h"
#include "undo/InsertUndoAction.h"
#include "gui/Cursor.h"
#include <cmath>


BaseStrokeHandler::BaseStrokeHandler(XournalView* xournal, XojPageView* redrawable, PageRef page, bool flipShift, bool flipControl)
 : InputHandler(xournal, redrawable, page)
{
	XOJ_INIT_TYPE(BaseStrokeHandler);
	
	this->flipShift = flipShift;
	this->flipControl = flipControl;
	
}

void BaseStrokeHandler::snapToGrid(double& x, double& y)
{
	XOJ_CHECK_TYPE(BaseStrokeHandler);

	if (!xournal->getControl()->getSettings()->isSnapGrid())
	{
		return;
	}

	/**
	 * Snap points to a grid:
	 * If x/y coordinates are under a certain tolerance,
	 * fix the point to the grid intersection value
	 */
	double gridSize = 14.17;

	double t = xournal->getControl()->getSettings()->getSnapGridTolerance();
	double tolerance = (gridSize / 2) - (1 / t);

	double xRem = fmod(x, gridSize);
	double yRem = fmod(y, gridSize);

	bool snapX = false;
	bool snapY = false;

	double tmpX = 0;
	double tmpY = 0;

	if (xRem < tolerance)
	{
		tmpX = x - xRem;
		snapX = true;
	}
	if (xRem > gridSize - tolerance )
	{
		tmpX = x + (gridSize - xRem);
		snapX = true;
	}
	if (yRem < tolerance)
	{
		tmpY = y - yRem;
		snapY = true;
	}
	if (yRem > gridSize - tolerance )
	{
		tmpY = y + (gridSize - yRem);
		snapY = true;
	}

	if (snapX && snapY)
	{
		x = tmpX;
		y = tmpY;
	}
}

BaseStrokeHandler::~BaseStrokeHandler()
{
	XOJ_CHECK_TYPE(BaseStrokeHandler);

	XOJ_RELEASE_TYPE(BaseStrokeHandler);
}

void BaseStrokeHandler::draw(cairo_t* cr)
{
	XOJ_CHECK_TYPE(BaseStrokeHandler);

	double zoom = xournal->getZoom();
	int dpiScaleFactor = xournal->getDpiScaleFactor();

	cairo_scale(cr, zoom * dpiScaleFactor, zoom * dpiScaleFactor);
	view.drawStroke(cr, stroke, 0);
}

bool BaseStrokeHandler::onKeyEvent(GdkEventKey* event) 
{
		if(event->is_modifier)
		{
			Rectangle rect = stroke->boundingRect();
				
			PositionInputData pos;
			pos.x = pos.y = pos.pressure = 0; //not used in redraw
			if( event->keyval == GDK_KEY_Shift_L || event->keyval == GDK_KEY_Shift_R)
			{
				pos.state = (GdkModifierType)(event->state ^ GDK_SHIFT_MASK);	// event state does not include current this modifier keypress - so ^toggle will work for press and release.
			}
			else if( event->keyval == GDK_KEY_Control_L || event->keyval == GDK_KEY_Control_R)
			{
				pos.state = (GdkModifierType)(event->state ^ GDK_CONTROL_MASK);
			}
			else if( event->keyval == GDK_KEY_Alt_L || event->keyval == GDK_KEY_Alt_R)
			{
				pos.state = (GdkModifierType)(event->state ^ GDK_MOD1_MASK);
			} 				
				
				
			this->redrawable->repaintRect(stroke->getX(), stroke->getY(), stroke->getElementWidth(), stroke->getElementHeight()); 	

			
			Point malleablePoint = this->currPoint;		//make a copy as it might get snapped to grid.
			this->drawShape( malleablePoint, pos );
	
			
			
			rect.add(stroke->boundingRect());
			
			double w = stroke->getWidth();
			redrawable->repaintRect(rect.x - w, rect.y - w, rect.width + 2 * w, rect.height + 2 * w);
	
	
		}
}

bool BaseStrokeHandler::onMotionNotifyEvent(const PositionInputData& pos)
{
	XOJ_CHECK_TYPE(BaseStrokeHandler);

	if (!stroke)
	{
		return false;
	}

	double zoom = xournal->getZoom();
	double x = pos.x / zoom;
	double y = pos.y / zoom;
	int pointCount = stroke->getPointCount();

	Point currentPoint(x, y);
	Rectangle rect = stroke->boundingRect();

	if (pointCount > 0)
	{
		if (!validMotion(currentPoint, stroke->getPoint(pointCount - 1)))
		{
			return true;
		}
	}

	this->redrawable->repaintRect(stroke->getX(), stroke->getY(),
								  stroke->getElementWidth(), stroke->getElementHeight());

	drawShape(currentPoint, pos);
	
	rect.add(stroke->boundingRect());
	double w = stroke->getWidth();

	redrawable->repaintRect(rect.x - w, rect.y - w,
							rect.width + 2 * w,
							rect.height + 2 * w);

	return true;
}

void BaseStrokeHandler::onButtonReleaseEvent(const PositionInputData& pos)
{
	XOJ_CHECK_TYPE(BaseStrokeHandler);

	if (stroke == NULL)
	{
		return;
	}

	// This is not a valid stroke
	if (stroke->getPointCount() < 2)
	{
		g_warning("Stroke incomplete!");

		delete stroke;
		stroke = NULL;
		return;
	}

	stroke->freeUnusedPointItems();


	Control* control = xournal->getControl();
	control->getLayerController()->ensureLayerExists(page);

	Layer* layer = page->getSelectedLayer();

	UndoRedoHandler* undo = control->getUndoRedoHandler();

	undo->addUndoAction(new InsertUndoAction(page, layer, stroke));

	layer->addElement(stroke);
	page->fireElementChanged(stroke);

	stroke = NULL;

	xournal->getCursor()->updateCursor();
	
	return;
}

void BaseStrokeHandler::onButtonPressEvent(const PositionInputData& pos)
{
	XOJ_CHECK_TYPE(BaseStrokeHandler);
	
	double zoom = xournal->getZoom();
	double x = pos.x / zoom;
	double y = pos.y / zoom;

	if (!stroke)
	{
		createStroke(Point(x, y));
	}
}


void BaseStrokeHandler::modifyModifiersByDrawDir(double width, double height,  bool changeCursor)
{
	XOJ_CHECK_TYPE(BaseStrokeHandler);
		

	bool gestureShift = this->flipShift;
	bool gestureControl = this->flipControl;
	
	if( this->drawModifierFixed == NONE){		//User hasn't dragged out past DrawDirModsRadius  i.e. modifier not yet locked.
		gestureShift = (width  < 0) != gestureShift;
		gestureControl =  (height < 0 ) != gestureControl;
		
		this->modShift = this->modShift ==  !gestureShift;
		this->modControl = this->modControl == !gestureControl;	
		
		double zoom = xournal->getZoom();
		double fixate_Dir_Mods_Dist = std::pow( xournal->getControl()->getSettings()->getDrawDirModsRadius() / zoom, 2.0); 
		if (std::pow(width,2.0) > fixate_Dir_Mods_Dist ||  std::pow(height,2.0) > fixate_Dir_Mods_Dist )
		{
			this->drawModifierFixed = (DIRSET_MODIFIERS)(SET |
				(gestureShift? SHIFT:NONE) |
				(gestureControl? CONTROL:NONE) );
			if(changeCursor)
			{
				xournal->getCursor()->updateCursor();
			}
		}
		else
		{
			if (changeCursor)
			{
				int corner = ( this->modShift?0:1 ) + ( this->modControl?2:0);
				
				if( corner != this-> lastCursor)
				{
					xournal->getCursor()->setTempDrawDirCursor(  this->modShift, this->modControl);
					this->lastCursor = corner;
				}
			}
		}
	}
	else
	{
		gestureShift = gestureShift == !(this->drawModifierFixed & SHIFT);
		gestureControl = gestureControl == !(this->drawModifierFixed & CONTROL);
		this->modShift = this->modShift ==  !gestureShift;
		this->modControl = this->modControl == !gestureControl;	
	}


		
}
