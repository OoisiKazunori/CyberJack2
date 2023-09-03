#pragma once

//主にステージセレクト画面のクラス
class StageSelectScene {
public:
	StageSelectScene();

	void Init();
	void Finalize();
	void Update();
	void Draw()const;

	void Input(bool Left, bool Right, bool Done);
	int StageNumber();
	bool SceneChange();
	bool TitleBack();

	bool _titleBackFlag;

private:
	int _stageNumber;
	bool _sceneChange;
	bool _titleSceneFlag;
	

	bool _timeFlag;
	int _timer;

	bool leftFlag, rightFlag, doneFlag;

};

