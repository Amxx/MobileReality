#include "configuration.hh"

static std::string format(std::string str, size_t length, size_t prefix = 5)
{
	size_t size = str.size();
	size_t postfix = length-prefix-3;
	if (size < length) return str;
	std::string result;
	result.append(str.substr(0, prefix));
	result.append("...");
	result.append(str.substr(size-postfix, postfix));
	return result;
}






Configuration::Configuration() :
	object(),
	devices(),
	markers(),
	general()
{
}

Configuration& Configuration::load(const std::string& path)
{
	// ------------------------------------------------------------------------------------------------------------
	cv::FileStorage fs(path, cv::FileStorage::READ);
	if (!fs.isOpened()) return *this;
	// ------------------------------------------------------------------------------------------------------------
	if (!fs["object"]["file"].empty())																					fs["object"]["file"]																								>>	object.file;
	if (!fs["object"]["scale"].empty())																					fs["object"]["scale"]																								>>	object.scale;
	if (!fs["object"]["material"]["kd"].empty())																fs["object"]["material"]["kd"]																			>>	object.material.kd;
	if (!fs["object"]["material"]["ks"].empty())																fs["object"]["material"]["ks"]																			>>	object.material.ks;
	if (!fs["object"]["material"]["ns"].empty())																fs["object"]["material"]["ns"]																			>>	object.material.ns;
	if (!fs["object"]["visibility"].empty())																		fs["object"]["visibility"]																					>>	object.visibility;			
	// ------------------------------------------------------------------------------------------------------------
	devices.front.enable = !fs["devices"]["front"].empty() && strcmp(((std::string) fs["devices"]["front"]["enable"]).c_str(), "off");
	if (fs["devices"]["front"]["id"].isInt())																		fs["devices"]["front"]["id"]																				>>	devices.front.id;
	else if	(fs["devices"]["front"]["id"].isString() && !strcmp(((std::string) fs["devices"]["front"]["id"]).c_str(), "auto")) 													devices.front.id = -1;
	if (!fs["devices"]["front"]["params"].empty())															fs["devices"]["front"]["params"]																		>>	devices.front.params;

	devices.back.enable = !fs["devices"]["back"].empty() && strcmp(((std::string) fs["devices"]["back"]["enable"]).c_str(), "off");
	if (fs["devices"]["back"]["id"].isInt())																		fs["devices"]["back"]["id"]																					>>	devices.back.id;
	else if	(fs["devices"]["back"]["id"].isString() && !strcmp(((std::string) fs["devices"]["back"]["id"]).c_str(), "auto"))														devices.front.id = -1;
	if (!fs["devices"]["back"]["params"].empty())																fs["devices"]["back"]["params"]																			>>	devices.back.params;
	
	// ------------------------------------------------------------------------------------------------------------
	if (!fs["markers"]["size"].empty())																					fs["markers"]["size"]																								>>	markers.size;
	if (!fs["markers"]["scale"].empty())																				fs["markers"]["scale"]																							>>	markers.scale;
	// ------------------------------------------------------------------------------------------------------------
	if (fs["generalParameters"]["verbose"].isInt())															fs["generalParameters"]["verbose"]																	>>	general.verbose;
	else if	(fs["generalParameters"]["verbose"].isString() && !strcmp(((std::string) fs["generalParameters"]["verbose"]).c_str(), "off")) 							general.verbose = 0;
	else if	(fs["generalParameters"]["verbose"].isString() && !strcmp(((std::string) fs["generalParameters"]["verbose"]).c_str(), "low"))								general.verbose = 1;
	else if	(fs["generalParameters"]["verbose"].isString() && !strcmp(((std::string) fs["generalParameters"]["verbose"]).c_str(), "high"))							general.verbose = 2;
	if (!fs["generalParameters"]["defaultValues"]["brightness"].empty())				fs["generalParameters"]["defaultValues"]["brightness"]							>>	general.defaultValues.brightness;
	if (!fs["generalParameters"]["defaultValues"]["gain"].empty())							fs["generalParameters"]["defaultValues"]["gain"]										>>	general.defaultValues.gain;
	if (!fs["generalParameters"]["defaultValues"]["persistency"].empty())				fs["generalParameters"]["defaultValues"]["persistency"]							>>	general.defaultValues.persistency;
	general.envmap.type						= strcmp(((std::string) fs["generalParameters"]["envmap"]["type"]).c_str(), "debug")?Options::DYNAMIC:Options::DEBUG;
	if (!fs["generalParameters"]["envmap"]["size"].empty())											fs["generalParameters"]["envmap"]["size"]														>>	general.envmap.size;
	general.envmap.dual						= !strcmp(((std::string) fs["generalParameters"]["envmap"]["dual"]).c_str(), "on");
	if (!fs["generalParameters"]["envmap"]["path"].empty())											fs["generalParameters"]["envmap"]["path"]														>>	general.envmap.path;
	general.localisation.type			= strcmp(((std::string) fs["generalParameters"]["localisation"]["type"]).c_str(), "debug")?Options::DYNAMIC:Options::DEBUG;
	if (!fs["generalParameters"]["localisation"]["size"].empty())								fs["generalParameters"]["localisation"]["size"]											>>	general.localisation.size;	
	general.rendering.background	= strcmp(((std::string) fs["generalParameters"]["rendering"]["background"]).c_str(), "off");
	general.rendering.scene				= strcmp(((std::string) fs["generalParameters"]["rendering"]["scene"]).c_str(), "off");
	general.rendering.view				= strcmp(((std::string) fs["generalParameters"]["rendering"]["view"]).c_str(), "off");
	if (!fs["generalParameters"]["modules"]["scanner"].empty())									fs["generalParameters"]["modules"]["scanner"]												>> general.modules.scanner;
	if (!fs["generalParameters"]["modules"].empty())														fs["generalParameters"]["modules"]["video"]													>> general.modules.video;
	// ------------------------------------------------------------------------------------------------------------
	fs.release();
	return *this;
}

Configuration& Configuration::check()
{
	if (!devices.front.enable)
	{
		if (general.envmap.type == Options::DYNAMIC)
		{ general.envmap.type = Options::DEBUG;		fprintf(stderr, "[WARNING] envmap has been set to debug as front device is not enable\n");				}
		if (general.localisation.type == Options::DYNAMIC)
		{ general.localisation.type	= Options::DEBUG;		fprintf(stderr, "[WARNING] localisation has been set to debug as front device is not enable\n");	}
	}
	if (general.envmap.type == Options::DYNAMIC && !general.envmap.path.empty())
	{ fprintf(stderr, "[WARNING] envmap path as been discarded as type was set to dynamic\n");																													}
	return *this;
}

Configuration& Configuration::display()
{
	const_cast<const Configuration*>(this)->display();
	return * this;
}
const Configuration& Configuration::display() const
{
	printf("┌─────────────────────────────────────────────────────────────────────┐\n");
	printf("│             C O N F I G U R A T I O N   O V E R V I E W             │\n");
	printf("├─────────────────────────────────────────────────────────────────────┤\n");
	printf("│ object.file                       : %-31s │\n",												format(object.file, 31, 8).c_str());
	printf("│ object.scale                      : %-31f │\n",												object.scale);
	printf("│ object.material.kd                : %-31f │\n",												object.material.kd);
	printf("│ object.material.ks                : %-31f │\n",												object.material.ks);
	printf("│ object.material.ns                : %-31f │\n",												object.material.ns);
	printf("│ object.visibility                 : %-31s │\n",												format(object.visibility, 31, 8).c_str());
	printf("│                                                                     │\n");
	printf("│ devices.front.enable              : %-31s │\n",												(devices.front.enable?std::string("on"):std::string("off")).c_str());
	printf("│ devices.front.id                  : %-31d │\n",												devices.front.id);
	printf("│ devices.front.params              : %-31s │\n",												format(devices.front.params, 31, 8).c_str());
	printf("│ devices.back.enable               : %-31s │\n",												(devices.back.enable?std::string("on"):std::string("off")).c_str());
	printf("│ devices.back.id                   : %-31d │\n",												devices.back.id);
	printf("│ devices.back.params               : %-31s │\n",												format(devices.back.params, 31, 8).c_str());
	printf("│                                                                     │\n");
	printf("│ markers.size                      : %-31f │\n",												markers.size);
	printf("│ markers.scale                     : %-31f │\n",												markers.scale);
	printf("│                                                                     │\n");
	printf("│ general.verbose                   : %-31s │\n",												(general.verbose?std::string("on"):std::string("off")).c_str());
	printf("│ general.defaultValues.brightness  : %-31d │\n",												general.defaultValues.brightness );
	printf("│ general.defaultValues.gain        : %-31d │\n",												general.defaultValues.gain);
	printf("│ general.defaultValues.persistency : %-31d │\n",												general.defaultValues.persistency);
	printf("│ general.envmap.type               : %-31s │\n",												(general.envmap.type?std::string("dynamic"):std::string("debug")).c_str());
	printf("│ general.envmap.size               : [%4d x %-4d]                   │\n",	general.envmap.size.width, general.envmap.size.height);
	printf("│ general.envmap.dual               : %-31s │\n",												(general.envmap.dual?std::string("on"):std::string("off")).c_str());
	printf("│ general.envmap.path               : %-31s │\n",												format(general.envmap.path, 31, 8).c_str());
	printf("│ general.localisation.type         : %-31s │\n",												(general.localisation.type?std::string("dynamic"):std::string("debug")).c_str());
	printf("│ general.localisation.size         : %-31f │\n",												general.localisation.size);
	printf("│ general.rendering.background      : %-31s │\n",												(general.rendering.background?std::string("on"):std::string("off")).c_str());
	printf("│ general.rendering.scene           : %-31s │\n",												(general.rendering.scene?std::string("on"):std::string("off")).c_str());
	printf("│ general.rendering.view            : %-31s │\n",												(general.rendering.view?std::string("on"):std::string("off")).c_str());
	printf("│ general.modules.scanner           : %-31s │\n",												format(general.modules.scanner, 31, 8).c_str());
	printf("│ general.modules.video             : %-31s │\n",												format(general.modules.video, 31, 8).c_str());
	printf("└─────────────────────────────────────────────────────────────────────┘\n");
	return *this;
}