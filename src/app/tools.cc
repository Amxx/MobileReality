#include "tools.hh"

cv::Matx33f ModelView(Camera& camera, Scanner& scanner)
{
	cv::Matx33f				view	= cv::Matx33f();
	cv::Matx33f				model	= cv::Matx33f();
	for (Symbol& symbol : scanner.scan(camera.frame()))
		try {
			try 				{	model = Matx44to33(parseSymbolToModel(symbol.data, 0.0));		}
			catch (...)	{ model = parseMatx33f(symbol.data); 													}
			symbol.extrinsic(camera.A(), camera.K());
			view = Matx44to33(viewFromSymbol(symbol.rvec, symbol.tvec));
		} catch (const std::exception& e) {
			std::cout << "Invalid symbol, could not extract model informations from `" << symbol.data << "`" << std::endl;
			std::cout << "Exception : " << e.what() << std::endl;
		}	
	return camera.orientation().inv() * view * model;
}

cv::Matx33f ModelView(Camera& camera, std::vector<Symbol>& symbols)
{
	cv::Matx33f				view	= cv::Matx33f();
	cv::Matx33f				model	= cv::Matx33f();
	for (Symbol& symbol : symbols)
		try {
			try 				{	model = Matx44to33(parseSymbolToModel(symbol.data, 0.0));		}
			catch (...)	{ model = parseMatx33f(symbol.data); 													}
			symbol.extrinsic(camera.A(), camera.K());
			view = Matx44to33(viewFromSymbol(symbol.rvec, symbol.tvec));
		} catch (const std::exception& e) {
			std::cout << "Invalid symbol, could not extract model informations from `" << symbol.data << "`" << std::endl;
			std::cout << "Exception : " << e.what() << std::endl;
		}	
	return camera.orientation().inv() * view * model;
}