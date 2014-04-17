﻿#include "core.hh"

// ############################################################################
// #                                  MACROS                                  #
// ############################################################################

#define			LOGHERE					std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;
#define			NOW()						std::chrono::steady_clock::now();

typedef			std::chrono::steady_clock::time_point														timer;
typedef			std::chrono::duration<long int, std::ratio<1l, 1000000000l>>		duration;

std::string formatTimer(duration dt)
{
	char buffer[1024];
	sprintf(buffer, "%3d ms %03d ys %03d ns",
					std::chrono::duration_cast<std::chrono::milliseconds>(dt).count(),
					std::chrono::duration_cast<std::chrono::microseconds>(dt).count()%1000,
					std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count()%1000);
	return std::string(buffer);
}




// ############################################################################
// #                                 METHODS                                  #
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App(),
	_cameras(2)
{
	// ===============================================================
	// =          C O N F I G U R A T I O N   L O A D I N G          =
	// ===============================================================
	if (argc > 1) _config.load(argv[1]).check();
	if (_config.general.verbose) _config.display();

	_buildenvmap	= _config.general.envmap.type == Options::DYNAMIC;
	_renderoptions	= 0x0020;
	// ===============================================================
	// =                   L O A D   S C A N N E R                   =
	// ===============================================================
	_scanner	= Module<Scanner>::load(_config.general.modules.scanner);
	if (_scanner == nullptr) exit(1);
	
	// ===============================================================
	// =                   L O A D   C A M E R A S                   =
	// ===============================================================
	if (_config.devices.front.enable)
	{
		VideoDevice* videodevice = Module<VideoDevice>::load(_config.general.modules.video);
		if (videodevice == nullptr) exit(1);
		_cameras[0] = new Camera(videodevice, cv::Matx44f(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0));
		if (_config.devices.front.id != -2) _cameras[0]->open(_config.devices.front.id);
		_cameras[0]->UIOpen();
	}
	if (_config.devices.back.enable)
	{
		VideoDevice* videodevice = Module<VideoDevice>::load(_config.general.modules.video);
		if (videodevice == nullptr) exit(1);
		_cameras[1] = new Camera(videodevice,  cv::Matx44f(-1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0));
		if (_config.devices.back.id != -2) _cameras[1]->open(_config.devices.back.id);
		_cameras[1]->UIOpen();
	}
	
	// ===============================================================
	// =                  S E T U P   C A M E R A S                  =
	// ===============================================================
	if (_config.devices.front.enable || _config.devices.back.enable)
	{
		if (_cameras[0]) _cameras[0]->setParameter(VideoDevice::MODE, VideoDevice::AUTOEXPOSURE);
		if (_cameras[1]) _cameras[1]->setParameter(VideoDevice::MODE, VideoDevice::AUTOEXPOSURE);
		
		printf("Waiting for camera to set autoexposure ... "); fflush(stdout); sleep(1); printf("done\n");
	
		if (_config.devices.front.enable)
		{
			int brightness	= (_config.general.defaultValues.brightness<0)?_cameras[0]->getParameter(VideoDevice::BRIGHTNESS):_config.general.defaultValues.brightness;
			int gain				= (_config.general.defaultValues.gain      <0)?_cameras[0]->getParameter(VideoDevice::GAIN)      :_config.general.defaultValues.gain;
			_cameras[0]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
			_cameras[0]->setParameter(VideoDevice::BRIGHTNESS,	brightness);
			_cameras[0]->setParameter(VideoDevice::GAIN,				gain);
			if (_config.general.verbose)
			{
				printf("┌─────────────────────────────────────────────────────────────────────┐\n");
				printf("│                           C A M E R A   0                           │\n");
				printf("├─────────────────────────────────────────────────────────────────────┤\n");
				printf("│ brightness : %-54d │\n", brightness);
				printf("│ gain       : %-54d │\n", gain);
				printf("└─────────────────────────────────────────────────────────────────────┘\n");
				// _cameras[0]->showParameters();
			}
			_cameras[0]->UICalibrate(_config.devices.front.params, *_scanner);
		}
		if (_config.devices.back.enable)
		{
			int brightness	= (_config.general.defaultValues.brightness==-1)?_cameras[1]->getParameter(VideoDevice::BRIGHTNESS):_config.general.defaultValues.brightness;
			int gain				= (_config.general.defaultValues.gain      ==-1)?_cameras[1]->getParameter(VideoDevice::GAIN)      :_config.general.defaultValues.gain;
			_cameras[1]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
			_cameras[1]->setParameter(VideoDevice::BRIGHTNESS,	brightness);
			_cameras[1]->setParameter(VideoDevice::GAIN,				gain);
			if (_config.general.verbose)
			{
				printf("┌─────────────────────────────────────────────────────────────────────┐\n");
				printf("│                           C A M E R A   1                           │\n");
				printf("├─────────────────────────────────────────────────────────────────────┤\n");
				printf("│ brightness : %-54d │\n", brightness);
				printf("│ gain       : %-54d │\n", gain);
				printf("└─────────────────────────────────────────────────────────────────────┘\n");
				// _cameras[1]->showParameters();
			}
			_cameras[1]->UICalibrate(_config.devices.back.params, *_scanner);
		}
	}
	
	// ===============================================================
	// =                   R E A D Y   T O   R U N                   =
	// ===============================================================
	gk::AppSettings settings;
	settings.setGLVersion(4,1);
	// settings.setFullscreen();
	if(createWindow(800, 600, settings) < 0) closeWindow();
	
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	present();
	
}





// ############################################################################





Core::~Core()
{
	for (Camera* camera : _cameras) if (camera) camera->close();
	for (auto r : _GLResources)	r.second->release();
}





// ############################################################################





int Core::init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0); // WHITE
	// glClearColor(0.0, 0.0, 0.0, 1.0); // BLACK
	// glClearColor(0.1, 0.1, 0.1, 1.0); //BLACK!10

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// ===============================================================
	// =        L O A D   S H A D E R S   A S   P R O G R A M        =
	// ===============================================================
	gk::programPath("install/shaders");
	
	_GLResources["prg:background_frame"]	= gk::createProgram("app_background_frame.glsl");
	_GLResources["prg:background_envmap"]	= gk::createProgram("app_background_cubemap.glsl");
	_GLResources["prg:build_cubemap"]			= gk::createProgram("app_build_cubemap.glsl");
	_GLResources["prg:rendering_object"]	= gk::createProgram("app_rendering_object.glsl");
	_GLResources["prg:rendering_shadows"]	= gk::createProgram("app_rendering_shadows.glsl");
	
	if (_GLResources["prg:background_frame"]	== gk::GLProgram::null())	return -1;
	if (_GLResources["prg:background_envmap"]	== gk::GLProgram::null())	return -1;
	if (_GLResources["prg:build_cubemap"]			== gk::GLProgram::null())	return -1;
	if (_GLResources["prg:rendering_object"]	== gk::GLProgram::null())	return -1;
	if (_GLResources["prg:rendering_shadows"]	== gk::GLProgram::null())	return -1;
	
	glBindAttribLocation(_GLResources["prg:rendering_object"]->name,	0, "position");
	glBindAttribLocation(_GLResources["prg:rendering_object"]->name,	1, "normal");
	glBindAttribLocation(_GLResources["prg:rendering_object"]->name,	2, "texcoord");
		
	// ===============================================================
	// =          C R E A T E   F R A M E   T E X T U R E S          =
	// ===============================================================	
	_GLResources["tex:frame0"]				= (new gk::GLTexture())->createTexture2D(0, 640, 480);
	_GLResources["tex:frame1"]				= (new gk::GLTexture())->createTexture2D(1, 640, 480);
		
	// ===============================================================
	// =                  C R E A T E   E N V M A P                  =
	// ===============================================================	
	if (_config.general.envmap.type == Options::DEBUG && !_config.general.envmap.path.empty())
		_GLResources["tex:envmap"]	= (new gk::GLTexture())->createTextureCube(2, gk::readImageArray(_config.general.envmap.path.c_str(), 6));
	else
		_GLResources["tex:envmap"]	= (new gk::GLTexture())->createTextureCube(2, _config.general.envmap.size.width, _config.general.envmap.size.height);

	_envmap.init(	getGLResource<gk::GLProgram>("prg:build_cubemap"),
								getGLResource<gk::GLTexture>("tex:envmap")					);
	
	// ===============================================================
	// =                  S E A M L E S S   C U B E                  =
	// ===============================================================
	_GLResources["spl:linear"]		= gk::createLinearSampler();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	// ===============================================================
	// =                    C R E A T E   M E S H                    =
	// ===============================================================
	gk::Mesh *mesh = gk::MeshIO::readOBJ(_config.object.file);
	if (mesh == nullptr) return -1;
	_mesh = new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
	_mesh->createBuffer(0,	mesh->positions);
  _mesh->createBuffer(1,	mesh->texcoords);
	_mesh->createBuffer(2,	mesh->normals);
	_mesh->createIndexBuffer(mesh->indices);
	_meshGroups = mesh->groups;
	_meshBox		=	mesh->box;
	delete mesh;

	
	// ===============================================================
	// =           C R E A T E   M E S H   T E X T U R E S           =
	// ===============================================================
	for (const gk::MeshGroup& grp : _meshGroups)
	{
		if (!grp.material.diffuse_texture.empty())
			_GLResources[grp.material.diffuse_texture]	= (new gk::GLTexture())->createTexture2D(3, gk::readImage(grp.material.diffuse_texture));
		if (!grp.material.specular_texture.empty())
			_GLResources[grp.material.specular_texture]	= (new gk::GLTexture())->createTexture2D(4, gk::readImage(grp.material.specular_texture));
	}
	
	
	
	
	
	_debugviewpoint = gk::Orbiter(_meshBox);
	
	return 1;
}
  

// ############################################################################


int Core::quit()
{
	return 1;
}





// ############################################################################


int Core::draw()
{
	// ===============================================================
	// =                        C O N T E X T                        =
	// ===============================================================
	static	gk::Transform	tr_v;
	static	gk::Transform	tr_vp;
	
	static	gk::Transform	tr_mv;
	static	gk::Transform	tr_mvp;
	
	bool									position_fresh		= false;
	static	int						position_duration	= 0;												// Persistent : Persistency duration
	
	
	// ===============================================================
	// =               U S E R   I N T E R A C T I O N               =
	// ===============================================================
	timer t1 = NOW();
	
	processKeyboardEvent();
	int x, y;
	unsigned int button= SDL_GetRelativeMouseState(&x, &y);
	if (button & SDL_BUTTON(1))				_debugviewpoint.rotate(x, y);
  else if (button & SDL_BUTTON(2))	_debugviewpoint.move(float(x) / float(windowWidth()), float(y) / float(windowHeight()));
	else if (button & SDL_BUTTON(3))	_debugviewpoint.move(x);
	
	// ===============================================================
	// =                 G R A B I N G   I M A G E S                 =
	// ===============================================================
	timer t2 = NOW();
	
	if (_config.devices.front.enable)
	{
		_cameras[0]->grabFrame();
		glActiveTexture	(GL_TEXTURE0);
		glBindTexture		(getGLResource<gk::GLTexture>("tex:frame0")->target, _GLResources["tex:frame0"]->name);
		glTexSubImage2D	(getGLResource<gk::GLTexture>("tex:frame0")->target, 0, 0, 0, _cameras[0]->frame()->width, _cameras[0]->frame()->height, GL_BGR, GL_UNSIGNED_BYTE, _cameras[0]->frame()->imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	if (_config.devices.back.enable)
	{
		_cameras[1]->grabFrame();
		glActiveTexture	(GL_TEXTURE0);
		glBindTexture		(getGLResource<gk::GLTexture>("tex:frame1")->target, _GLResources["tex:frame1"]->name);
		glTexSubImage2D	(getGLResource<gk::GLTexture>("tex:frame1")->target, 0, 0, 0, _cameras[1]->frame()->width, _cameras[1]->frame()->height, GL_BGR, GL_UNSIGNED_BYTE, _cameras[1]->frame()->imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
	// ===============================================================
	// =                    P O S I T I O N I N G                    =
	// ===============================================================	
	timer t3 = NOW();
	
	switch (_config.general.localisation.type)
	{
		case Options::DYNAMIC:
		{
			static cv::Matx44f toGL(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
			cv::Matx44f model, view;
					
			for (Symbol& symbol : _scanner->scan(_cameras[0]->frame()))
				try {
					// LEGACY MODE FOR CUBE
					try 				{	model = parseSymbolToModel(	symbol.data, _config.markers.size															) * toGL;	}
					catch (...)	{ model = parseMatx33f_tr		(	symbol.data, _config.markers.size *cv::Matx31f(0.5, 0.5, 0.5)	) * toGL;	}
					symbol.extrinsic(_cameras[0]->A(), _cameras[0]->K(), Scanner::pattern(_config.markers.size, _config.markers.scale));
					view								=	_cameras[0]->orientation().inv() * viewFromSymbol(symbol.rvec, symbol.tvec) * model;
					position_fresh			= !isNull(view);
					if (position_fresh)
					{
						position_duration		= _config.general.defaultValues.persistency;
						gk::Transform	proj	= cv2gkit(projectionFromIntrinsic(_cameras[0]->A(), _cameras[0]->frame()->width, _cameras[0]->frame()->height, 1.f, 10000.f));
						tr_v								=	cv2gkit(view);
						tr_vp								= proj * cv2gkit(_cameras[0]->orientation()) * tr_v;
						break;
					}
				} catch (...) {
					printf("Invalid symbol, could not extract model informations from `%s`\n", symbol.data.c_str());
				}
			break;
		}
		case Options::DEBUG:
		{
			gk::Transform proj	= gk::Perspective(45.f, (float) windowWidth()/windowHeight(), 0.1f, 1000000.f);
			tr_v								= _debugviewpoint.view();
			tr_vp								= proj * tr_v;
			break;
		}
	}
	tr_mv		= tr_v	* gk::Scale(_config.object.scale);
	tr_mvp	= tr_vp	* gk::Scale(_config.object.scale);
	
	// ===============================================================
	// =                B U I L D I N G   E N V M A P                =
	// ===============================================================
	timer t4 = NOW();
		
	if (_buildenvmap && position_fresh)
	{
		if (_config.devices.back.enable) _envmap.cuberender(_cameras[1], tr_v, getGLResource<gk::GLTexture>("tex:frame1"), _config.general.localisation.size);
		if (_config.general.envmap.dual) _envmap.cuberender(_cameras[0], tr_v, getGLResource<gk::GLTexture>("tex:frame0"), _config.general.localisation.size);
		_envmap.generateMipMap();
	}

	// ===============================================================
	// =                      R E N D E R I N G                      =
	// ===============================================================
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth(), windowHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// ===============================================================
	// =               B A C K G R O U N D   F R A M E               =
	// ===============================================================
	timer t5 = NOW();
	
	if (_config.general.rendering.background)
	{
		glClear(GL_DEPTH_BUFFER_BIT);	
		
		switch(_config.general.localisation.type)
		{
			case Options::DYNAMIC:
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(getGLResource<gk::GLTexture>("tex:frame0")->target, _GLResources["tex:frame0"]->name);
				glUseProgram(_GLResources["prg:background_frame"]->name);
				getGLResource<gk::GLProgram>("prg:background_frame")->sampler("frame") = 0;
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				break;
			}
			case Options::DEBUG:
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(getGLResource<gk::GLTexture>("tex:envmap")->target, _GLResources["tex:envmap"]->name);
				glBindSampler(0, _GLResources["spl:linear"]->name);
				glUseProgram(_GLResources["prg:background_envmap"]->name);
				getGLResource<gk::GLProgram>("prg:background_envmap")->uniform("reproject")	= tr_vp.inverseMatrix();
				getGLResource<gk::GLProgram>("prg:background_envmap")->sampler("envmap")		= 0;
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				break;
			}
		}
	}
	
	// ===============================================================
	// =                S C E N E   R E N D E R I N G                =
	// ===============================================================
	timer t6 = NOW();
	
	if (_config.general.rendering.scene && (_config.general.localisation.type == Options::DEBUG || (position_duration && position_duration--)))
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(getGLResource<gk::GLTexture>("tex:envmap")->target, _GLResources["tex:envmap"]->name);
		glBindSampler(0, _GLResources["spl:linear"]->name);
		
		if (_renderoptions & 0x0020)
		{
			gk::Point box_center;
			float			box_radius;
			_meshBox.BoundingSphere(box_center, box_radius);
			
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glUseProgram(_GLResources["prg:rendering_shadows"]->name);
			getGLResource<gk::GLProgram>("prg:rendering_shadows")->uniform("mvpMatrix")			= tr_mvp.matrix();
			getGLResource<gk::GLProgram>("prg:rendering_shadows")->uniform("method")				= _renderoptions;
			getGLResource<gk::GLProgram>("prg:rendering_shadows")->sampler("envmap")				= 0;
			getGLResource<gk::GLProgram>("prg:rendering_shadows")->uniform("bbox_radius")		=	box_radius;	
			getGLResource<gk::GLProgram>("prg:rendering_shadows")->uniform("bbox_height")		= _meshBox.pMin.y;
			
			getGLResource<gk::GLProgram>("prg:rendering_shadows")->uniform("sphere_center") = box_center;							//gk::Vec3(0.f, 0.f, 0.f);
			getGLResource<gk::GLProgram>("prg:rendering_shadows")->uniform("sphere_size") 	= box_radius/sqrtf(3.f);	//+1.f;
			
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glDisable(GL_BLEND);
		}
		
		glClear(GL_DEPTH_BUFFER_BIT);
		glUseProgram(_GLResources["prg:rendering_object"]->name);
		getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("mvMatrix")			= tr_mv.matrix();
		getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("mvMatrixInv")	= tr_mv.inverseMatrix();		
		getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("mvpMatrix")		= tr_mvp.matrix();
		getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("method")				= _renderoptions;
		getGLResource<gk::GLProgram>("prg:rendering_object")->sampler("envmap")				= 0;
		
		for (const gk::MeshGroup& grp : _meshGroups)
		{
			if (!grp.material.diffuse_texture.empty())
			{
				glActiveTexture(GL_TEXTURE0+1);
				glBindTexture(getGLResource<gk::GLTexture>(grp.material.diffuse_texture)->target, _GLResources[grp.material.diffuse_texture]->name);
			}
			
			if (!grp.material.specular_texture.empty())
			{
				glActiveTexture(GL_TEXTURE0+2);
				glBindTexture(getGLResource<gk::GLTexture>(grp.material.specular_texture)->target, _GLResources[grp.material.specular_texture]->name);
			}

			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("diffuse_color")				= grp.material.diffuse_color;
			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("use_diffuse_texture")	= !grp.material.diffuse_texture.empty();
			getGLResource<gk::GLProgram>("prg:rendering_object")->sampler("diffuse_texture") 			= 1;
			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("kd")										= grp.material.kd;

			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("specular_color")				= grp.material.specular_color;
			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("use_specular_texture") = !grp.material.specular_texture.empty();
			getGLResource<gk::GLProgram>("prg:rendering_object")->sampler("specular_texture") 		= 2;
			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("ks")										= grp.material.ks;
			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("ns")										= grp.material.ns;
			
			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("emission")							= grp.material.emission;
			getGLResource<gk::GLProgram>("prg:rendering_object")->uniform("ni")										= grp.material.ni;
			
			_mesh->drawGroup(grp.begin, grp.end);
		}
		
	
	}
	
	// ===============================================================
	// =                 F R A M E S   D I S P L A Y                 =
	// ===============================================================
	timer t7 = NOW();
	
	if (_config.general.rendering.view)
	{
		if (_config.devices.front.enable)	cv::imshow("camera 0", cv::Mat(_cameras[0]->frame()));
		if (_config.devices.back.enable)	cv::imshow("camera 1", cv::Mat(_cameras[1]->frame()));
	}
	
	// ===============================================================
	// =                         T I M E R S                         =
	// ===============================================================
	timer t8 = NOW();
	
	if (_config.general.verbose > 1)
	{
		cv::Mat timers(140, 280, CV_8UC1, cv::Scalar(25,25,25));
		putText(timers, "User interaction",				cv::Point(5,10),  0, 0.36, cv::Scalar(230,230,230));	putText(timers, formatTimer(t2-t1), cv::Point(155,10),  0, 0.32, cv::Scalar(230,230,230));	
		putText(timers, "Grabbing frames",				cv::Point(5,30),  0, 0.36, cv::Scalar(230,230,230));	putText(timers, formatTimer(t3-t2), cv::Point(155,30),  0, 0.32, cv::Scalar(230,230,230));
		putText(timers, "Positionning",						cv::Point(5,50),  0, 0.36, cv::Scalar(230,230,230));	putText(timers, formatTimer(t4-t3), cv::Point(155,50),  0, 0.32, cv::Scalar(230,230,230));
		putText(timers, "Bilding EnvMap",					cv::Point(5,70),  0, 0.36, cv::Scalar(230,230,230));	putText(timers, formatTimer(t5-t4), cv::Point(155,70),  0, 0.32, cv::Scalar(230,230,230));
		putText(timers, "Rendering - background",	cv::Point(5,90),  0, 0.36, cv::Scalar(230,230,230));	putText(timers, formatTimer(t6-t5), cv::Point(155,90),  0, 0.32, cv::Scalar(230,230,230));
		putText(timers, "Rendering - scene",			cv::Point(5,110), 0, 0.36, cv::Scalar(230,230,230));	putText(timers, formatTimer(t7-t6), cv::Point(155,110), 0, 0.32, cv::Scalar(230,230,230));
		putText(timers, "Rendering - frames",			cv::Point(5,130), 0, 0.36, cv::Scalar(230,230,230));	putText(timers, formatTimer(t8-t7), cv::Point(155,130), 0, 0.32, cv::Scalar(230,230,230));
		cv::imshow("timers", timers);
	}
	
	present();
	cv::waitKey(3);
	return 1;
}





// ############################################################################


void Core::processKeyboardEvent()
{
	if (key('b') && !(key('b')=0) )	{		_buildenvmap = !_buildenvmap;			if (_config.general.verbose) printf("- build envmap : %s\n",									_buildenvmap?"on":"off"																															);	}
	if (key('c') && !(key('c')=0) )	{		_envmap.clear();									if (_config.general.verbose) printf("- envmap cleared\n"																																																					);	}
	if (key('r') && !(key('r')=0) )	{		_renderoptions ^= 0x0001;					if (_config.general.verbose) printf("- switch to %s object render methode\n", ((_renderoptions & 0x0001)?std::string("old"):std::string("new")).c_str()						);	}
	if (key('s') && !(key('s')=0) )	{		_renderoptions ^= 0x0010;					if (_config.general.verbose) printf("- switch to %s shadow render methode\n", ((_renderoptions & 0x0010)?std::string("old"):std::string("new")).c_str()						);	}
	if (key('d') && !(key('d')=0) )	{		_renderoptions ^= 0x0020;					if (_config.general.verbose) printf("- shadow rendering %s\n",                ((_renderoptions & 0x0020)?std::string("enabled"):std::string("disabled")).c_str()	);	}	
}


// ############################################################################


void Core::processKeyboardEvent(SDL_KeyboardEvent& event)
{	
	if (event.state == SDL_PRESSED)
		switch (event.keysym.sym)
		{
			case SDLK_F1:
			{
				std::stringstream path;
				path << "data/view/screenshot_" << time(0) << ".png";
				gk::writeFramebuffer(path.str());
				break;
			}
			case SDLK_F2:
			{
				std::stringstream path;
				path << "data/view/envmap_" << time(0) << "_%03d.png";
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(getGLResource<gk::GLTexture>("tex:envmap")->target, _GLResources["tex:envmap"]->name);
				gk::ImageIO::writeImageArray(path.str(), getGLResource<gk::GLTexture>("tex:envmap")->imageArray(0));
				break;
			}
			case SDLK_F5:
			{
				gk::reloadPrograms();
				break;
			}
			case SDLK_F9:
			{
				gk::writeOrbiter(_debugviewpoint, ".debugviewpoint.save");
				break;
			}
			case SDLK_F10:
			{
				_debugviewpoint = gk::readOrbiter(".debugviewpoint.save");
				break;
			}
			case SDLK_ESCAPE:
			{
				closeWindow();
				break;
			}
			case SDLK_RIGHT:
			{
				int brightness;
				if (_config.devices.front.enable)
				{
					brightness = std::min(_cameras[0]->getParameter(VideoDevice::BRIGHTNESS) + 8, 255);
					_cameras[0]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					if (_config.general.verbose) printf("[CAMERA %d] brightness set to %d\n", 0, brightness);
				}
				if (_config.devices.back.enable)
				{
					brightness = std::min(_cameras[1]->getParameter(VideoDevice::BRIGHTNESS) + 8, 255);
					_cameras[1]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					if (_config.general.verbose) printf("[CAMERA %d] brightness set to %d\n", 1, brightness);
				}
				break;
			}
			case SDLK_LEFT:
			{
				if (_config.devices.front.enable)
				{
					int brightness = std::max(_cameras[0]->getParameter(VideoDevice::BRIGHTNESS) - 8, 0);
					_cameras[0]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					if (_config.general.verbose) printf("[CAMERA %d] brightness set to %d\n", 0, brightness);
				}
				if (_config.devices.back.enable)
				{
					int brightness = std::max(_cameras[1]->getParameter(VideoDevice::BRIGHTNESS) - 8, 0);
					_cameras[1]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					if (_config.general.verbose) printf("[CAMERA %d] brightness set to %d\n", 1, brightness);
				}
				break;
			}
			case SDLK_UP:
			{
				if (_config.devices.front.enable)
				{
					int gain = std::min(_cameras[0]->getParameter(VideoDevice::GAIN) + 8, 255);
					_cameras[0]->setParameter(VideoDevice::GAIN, gain);
					if (_config.general.verbose) printf("[CAMERA %d] gain set to %d\n", 0, gain);
				}
				if (_config.devices.back.enable)
				{
					int gain = std::min(_cameras[1]->getParameter(VideoDevice::GAIN) + 8, 255);
					_cameras[1]->setParameter(VideoDevice::GAIN, gain);
					if (_config.general.verbose) printf("[CAMERA %d] gain set to %d\n", 1, gain);
				}
				break;
			}
			case SDLK_DOWN:
			{
				if (_config.devices.front.enable)
				{
					int gain = std::max(_cameras[0]->getParameter(VideoDevice::GAIN) - 8, 0);
					_cameras[0]->setParameter(VideoDevice::GAIN, gain);
					if (_config.general.verbose) printf("[CAMERA %d] gain set to %d\n", 0, gain);
				}
				if (_config.devices.back.enable)
				{
					int gain = std::max(_cameras[1]->getParameter(VideoDevice::GAIN) - 8, 0);
					_cameras[1]->setParameter(VideoDevice::GAIN, gain);
					if (_config.general.verbose) printf("[CAMERA %d] gain set to %d\n", 1, gain);
				}
				break;
			}
			default:
				if (_config.general.verbose) printf("Key %d unmapped\n", (int) event.keysym.sym);
				break;
		}
}





// ############################################################################





void Core::processWindowResize(SDL_WindowEvent &event)
{
}





// ############################################################################





void Core::processMouseButtonEvent(SDL_MouseButtonEvent &event)
{	
}





// ############################################################################





void Core::processMouseMotionEvent(SDL_MouseMotionEvent &event)
{
}	

