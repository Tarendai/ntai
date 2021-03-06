/*
AF 2007
*/

class CMessage {
public:
	CMessage(std::string my_type);
	CMessage(std::string my_type, std::vector<float> &myparameters);

	std::vector<float> GetParameters();

	float GetParameter(int i);
	void* GetOtherParameters();

	std::string GetType();
	bool IsType(std::string type);
	void SetType(std::string type);

	int GetFrame();
	void SetFrame(int frame);

	
	float3 GetFloat3();
	float GetRadius();

	void AddParameter(int p);
	void AddParameter(float p);
	void AddParameter(float3 p);

	void SetLifeTime(int lifetime);
	bool IsDead(int currentFrame);
	int GetLifeTime();
	int GetRemainingLifeTime(int currentFrame);

private:
	int frame;
	int lifetime; // -1 indefinate
	std::string message_type;
	std::vector<float> parameters;
	void* otherstuff;
};
