///////////////////////////////////////////////////////////////////////////////
// Filename: GameTest.cpp
// Provides a demo of how to use the API
///////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
#include <algorithm>
//------------------------------------------------------------------------
#include "app\app.h"
#include "ECS\SceneSideScroll.h"
#include "ECS\SceneThirdPerson.h"
#include "ECS\SceneGolf.h"

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Example data....
//------------------------------------------------------------------------
//SceneSideScroll scene1;
//SceneThirdPerson scene2;
SceneGolf scene3;
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	//scene1.Init();
	//scene2.Init();
	scene3.Init();
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(const float deltaTime)
{
	//scene1.Update(deltaTime);
	//scene2.Update(deltaTime);
	if (!App::IsSoundPlaying("Data/background.wav"))
	{
		App::PlaySoundW("Data/background.wav", true);
	}
	scene3.Update(deltaTime);
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{	
	//scene1.Render();
	//scene2.Render();
	scene3.Render();
}
//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{	

}