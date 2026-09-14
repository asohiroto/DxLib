#include "SceneManager.h"
#include "SceneMain.h"

SceneManager::SceneManager():
    p_Main(nullptr),
    _nowScene(SceneType::Main)
{
}

SceneManager::~SceneManager() 
{
}

void SceneManager::Init() 
{
    p_Main = std::make_unique<SceneMain>();
    p_Main->Init();
}

void SceneManager::End() 
{
    if (_nowScene == SceneType::Main) {
        p_Main->End();
    }
}

void SceneManager::Update() 
{    
    if (_nowScene == SceneType::Main) {
        p_Main->Update();
    }
}

void SceneManager::Draw() 
{    
    if (_nowScene == SceneType::Main) {
        p_Main->Draw();
    }
}
