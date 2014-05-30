/*
Copyright or © or Copr. Hadrien Croubois (2014)

hadrien.croubois@ens-lyon.fr

This software is a computer program whose purpose is to render, in real
time, virtual objects usgin dynamically acquired environnement.

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#include "scanner_zbar.hh"


extern "C" {
	Module<Scanner>* maker() { return new scanners::ZBar(); }
}

scanners::ZBar::ZBar():
	scanner()
{
	scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
}

std::vector<Symbol> scanners::ZBar::scan(IplImage* img)
{
	cv::Mat frame(img), frame_grayscale;
	cvtColor(frame, frame_grayscale, CV_BGR2GRAY);		
	
	int width		= frame_grayscale.cols;
	int height	= frame_grayscale.rows;
	uchar *raw	= (uchar *)(frame_grayscale.data);
	
	zbar::Image zframe(width, height, "Y800", raw, width * height);
	scanner.scan(zframe);
		
	std::vector<Symbol> results;
	
	for (zbar::Image::SymbolIterator symbol = zframe.symbol_begin(); symbol != zframe.symbol_end(); ++symbol)
	{
		Symbol symbolinfos;
		for (int i = 0; i<symbol->get_location_size(); ++i)
			symbolinfos.pts.push_back(cv::Matx21f(symbol->get_location_x(i), symbol->get_location_y(i)));
		symbolinfos.data = symbol->get_data();	
		results.push_back(symbolinfos);
	}
	return results;
}