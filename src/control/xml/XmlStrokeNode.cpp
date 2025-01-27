#include "Util.h"
#include "XmlStrokeNode.h"

XmlStrokeNode::XmlStrokeNode(const char* tag) : XmlNode(tag)
{
	XOJ_INIT_TYPE(XmlStrokeNode);

	this->points = NULL;
	this->pointsLength = 0;
	this->width = 0;
	this->widths = NULL;
	this->widthsLength = 0;
}

XmlStrokeNode::~XmlStrokeNode()
{
	XOJ_CHECK_TYPE(XmlStrokeNode);

	delete[] this->points;
	delete[] this->widths;

	XOJ_RELEASE_TYPE(XmlStrokeNode);
}

void XmlStrokeNode::setPoints(Point* points, int pointsLength)
{
	XOJ_CHECK_TYPE(XmlStrokeNode);

	if (this->points)
	{
		delete[] this->points;
	}
	this->points = new Point[pointsLength];
	for (int i = 0; i < pointsLength; i++)
	{
		this->points[i] = points[i];
	}
	this->pointsLength = pointsLength;
}

void XmlStrokeNode::setWidth(double width, double* widths, int widthsLength)
{
	XOJ_CHECK_TYPE(XmlStrokeNode);

	this->width = width;

	if (this->widths)
	{
		delete[] this->widths;
	}
	this->widths = new double[widthsLength];
	for (int i = 0; i < widthsLength; i++)
	{
		this->widths[i] = widths[i];
	}
	this->widthsLength = widthsLength;

}

void XmlStrokeNode::writeOut(OutputStream* out)
{
	XOJ_CHECK_TYPE(XmlStrokeNode);

	out->write("<");
	out->write(tag);
	writeAttributes(out);

	out->write(" width=\"");

	char widthStr[G_ASCII_DTOSTR_BUF_SIZE];
	// g_ascii_ version uses C locale always.
	g_ascii_formatd(widthStr, G_ASCII_DTOSTR_BUF_SIZE, Util::PRECISION_FORMAT_STRING, width);
	out->write(widthStr);

	for (int i = 0; i < widthsLength; i++)
	{
		g_ascii_formatd(widthStr, G_ASCII_DTOSTR_BUF_SIZE, Util::PRECISION_FORMAT_STRING, widths[i]);
		out->write(" ");
		out->write(widthStr);
	}

	out->write("\"");

	if (this->pointsLength == 0)
	{
		out->write("/>");
	}
	else
	{
		out->write(">");

		gchar coordinateStr[Util::PRECISION_FORMAT_BUFF_LEN];
		Util::getCoordinateString(coordinateStr, Util::PRECISION_FORMAT_BUFF_LEN, points[0].x, points[0].y);
		out->write(coordinateStr);

		for (int i = 1; i < this->pointsLength; i++)
		{
			Util::getCoordinateString(coordinateStr, Util::PRECISION_FORMAT_BUFF_LEN, points[i].x, points[i].y);
			out->write(" ");
			out->write(coordinateStr);
		}

		out->write("</");
		out->write(tag);
		out->write(">\n");
	}
}
