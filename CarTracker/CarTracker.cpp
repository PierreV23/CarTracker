#include "pch.h"
#include "CarTracker.h"


BAKKESMOD_PLUGIN(CarTracker, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
bool coolEnabled = false;

void CarTracker::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("CarTracker - Plugin loaded!");
	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("CarTracker debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	LOG("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	LOG("the cvar with name: {} changed", cvarName);
	//	LOG("the new value is: {}", newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&CarTracker::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&CarTracker::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	LOG("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&CarTracker::YourPluginMethod, this);
	cvarManager->registerNotifier("CoolerBallOnTop", [this](std::vector<std::string> args) {
		ballOnTop();
	}, "", PERMISSION_ALL);
	cvarManager->registerCvar("cool_enabled", "0", "Enable Cool", true, true, 0, true, 1);
	cvarManager->registerCvar("cool_distance", "200.0", "Distance to place the ball above");
	cvarManager->registerCvar("cool_enabled", "0", "Enable Cool", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		coolEnabled = cvar.getBoolValue();
			});

	// circle
	cvarManager->registerNotifier("DrawBall", [this](std::vector<std::string> args) {
		drawBall();
	}, "", PERMISSION_ALL);

	cvarManager->registerCvar("ct_coord_x", "577.0", "ct_coord_x");
	cvarManager->registerCvar("ct_coord_y", "363.0", "ct_coord_y");
	////gameWrapper->HookEvent("Function Engine.Interaction.Tick",
	//gameWrapper->HookEvent("Function TAGame.PlayerInput_TA.GetKeyForAction",
	//	[this](std::string eventName) {
	//		// spawn circle on cursor
	//		ImVec2 cursor = ImGui::GetMousePos();
	//		if (CarTracker::insideRocketLeagueWindow(cursor)) {
	//			CanvasWrapper
	//			//LOG("TEST");
	//			CarTracker::drawBallToPos(cursor);
	//		}
	//		CarTracker::drawBall();
	//	}
	//);
	CarTracker::drawBall();
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
	});

	// register points
	points.push_back("p1");
	points.push_back("p2");
	points.push_back("p3");

	for (auto& point : points) {
		cvarManager->registerCvar("ct_coord_" + point + "_x", "0.0", "value of ct_coord_" + point + "_x");
		cvarManager->registerCvar("ct_coord_" + point + "_y", "0.0", "value of ct_coord_" + point + "_y");
	}
	
}


void CarTracker::ballOnTop() {
	if (!gameWrapper->IsInFreeplay()) { return; }
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }

	BallWrapper ball = server.GetBall();
	if (!ball) { return; }
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) { return; }

	Vector carVelocity = car.GetVelocity();
	ball.SetVelocity(carVelocity);

	Vector carLocation = car.GetLocation();
	float ballRadius = ball.GetRadius();

	CVarWrapper distanceCVar = cvarManager->getCvar("cool_distance");
	if (!distanceCVar) { return; }
	float distance = distanceCVar.getFloatValue();

	ball.SetLocation(carLocation + Vector{ 0, 0, distance });
}


void CarTracker::RenderSettings() {
	ImGui::TextUnformatted("A really cool plugin");

	// spawn ball on top
	if (ImGui::Button("Ball On Top")) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			cvarManager->executeCommand("CoolerBallOnTop");
			});
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Activate Ball On Top");
	}

	// toggle
	CVarWrapper enableCvar = cvarManager->getCvar("cool_enabled");
	if (!enableCvar) { return; }
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Cool Plugin");
	}

	// distance
	CVarWrapper distanceCvar = cvarManager->getCvar("cool_distance");
	if (!distanceCvar) { return; }
	float distance = distanceCvar.getFloatValue();
	if (ImGui::SliderFloat("Distance", &distance, 0.0, 500.0)) {
		distanceCvar.setValue(distance);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "distance is " + std::to_string(distance);
		ImGui::SetTooltip(hoverText.c_str());
	}

	// respawn circle
	if (ImGui::Button("Circle")) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			cvarManager->executeCommand("DrawBall");
		});
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Respawn circle");
	}


	// x slider
	CVarWrapper cvar_x = cvarManager->getCvar("ct_coord_x");
	if (!cvar_x) { return; }
	float x = cvar_x.getFloatValue();
	if (ImGui::SliderFloat("x:", &x, 0.0, 1920.0)) {
		cvar_x.setValue(x);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "x is " + std::to_string(x);
		ImGui::SetTooltip(hoverText.c_str());
	}

	// y slider
	CVarWrapper cvar_y = cvarManager->getCvar("ct_coord_y");
	if (!cvar_y) { return; }
	float y = cvar_y.getFloatValue();
	if (ImGui::SliderFloat("y:", &y, 0.0, 1080.0)) {
		cvar_y.setValue(y);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "x is " + std::to_string(x);
		ImGui::SetTooltip(hoverText.c_str());
	}

	//ImVec2 cursor = ImGui::GetCursorPos();
	//ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
	ImVec2 cursor = ImGui::GetMousePos();
	//ImGui::TextUnformatted(std::to_string(cursor.x) + ';' + std::to_string(cursor.y));
	ImGui::SliderFloat("X", &cursor.x, 0.0, 1080.0);
	ImGui::SliderFloat("Y", &cursor.y, 0.0, 1080.0);
	
	// spawn circle
	CarTracker::drawBall();

	ImGui::TextUnformatted("A really cool plugin");

	// sliders for the points
	CVarWrapper p1_cvar_y = cvarManager->getCvar("ct_coord_p1_y");
	if (!p1_cvar_y) { return; }
	float p1_y = p1_cvar_y.getFloatValue();
	if (ImGui::SliderFloat("p1_y:", &p1_y, 0.0, 1080.0)) {
		p1_cvar_y.setValue(p1_y);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "p1_y is " + std::to_string(y);
		ImGui::SetTooltip(hoverText.c_str());
	}

	CVarWrapper p1_cvar_x = cvarManager->getCvar("ct_coord_p1_x");
	if (!p1_cvar_x) { return; }
	float p1_x = p1_cvar_x.getFloatValue();
	if (ImGui::SliderFloat("p1_x:", &p1_x, 0.0, 1920.0)) {
		p1_cvar_x.setValue(p1_x);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "p1_x is " + std::to_string(x);
		ImGui::SetTooltip(hoverText.c_str());
	}

	CVarWrapper p2_cvar_y = cvarManager->getCvar("ct_coord_p2_y");
	if (!p2_cvar_y) { return; }
	float p2_y = p2_cvar_y.getFloatValue();
	if (ImGui::SliderFloat("p2_y:", &p2_y, 0.0, 1080.0)) {
		p2_cvar_y.setValue(p2_y);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "p2_y is " + std::to_string(y);
		ImGui::SetTooltip(hoverText.c_str());
	}

	CVarWrapper p2_cvar_x = cvarManager->getCvar("ct_coord_p2_x");
	if (!p2_cvar_x) { return; }
	float p2_x = p2_cvar_x.getFloatValue();
	if (ImGui::SliderFloat("p2_x:", &p2_x, 0.0, 1920.0)) {
		p2_cvar_x.setValue(p2_x);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "p2_x is " + std::to_string(x);
		ImGui::SetTooltip(hoverText.c_str());
	}

	CVarWrapper p3_cvar_y = cvarManager->getCvar("ct_coord_p3_y");
	if (!p3_cvar_y) { return; }
	float p3_y = p3_cvar_y.getFloatValue();
	if (ImGui::SliderFloat("p3_y:", &p3_y, 0.0, 1080.0)) {
		p3_cvar_y.setValue(p3_y);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "p3_y is " + std::to_string(y);
		ImGui::SetTooltip(hoverText.c_str());
	}

	CVarWrapper p3_cvar_x = cvarManager->getCvar("ct_coord_p3_x");
	if (!p3_cvar_x) { return; }
	float p3_x = p3_cvar_x.getFloatValue();
	if (ImGui::SliderFloat("p3_x:", &p3_x, 0.0, 1920.0)) {
		p3_cvar_x.setValue(p3_x);
	}
	if (ImGui::IsItemHovered()) {
		std::string hoverText = "p3_x is " + std::to_string(x);
		ImGui::SetTooltip(hoverText.c_str());
	}
}

void CarTracker::drawBall()
{
	CVarWrapper cvar_x = cvarManager->getCvar("ct_coord_x");
	if (!cvar_x) { return; }
	float x = cvar_x.getFloatValue();

	CVarWrapper cvar_y = cvarManager->getCvar("ct_coord_y");
	if (!cvar_y) { return; }
	float y = cvar_y.getFloatValue();


	//ImDrawList* draw_list = ImGui::GetOverlayDrawList();
	//ImVec2 cursor = ImGui::GetCursorScreenPos();
	//LOG(std::to_string(cursor.x) + ';' + std::to_string(cursor.y));
	CarTracker::drawBallToPos(ImVec2(x, y));
}

void CarTracker::drawBallToPos(ImVec2 pos)
{
	ImDrawList* draw_list = ImGui::GetOverlayDrawList();
	static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
	const ImU32 col = ImColor(colf);
	draw_list->AddCircleFilled(pos, 10.0f, col);
}

bool CarTracker::insideRocketLeagueWindow(ImVec2 pos)
{
	float x = pos.x;
	float y = pos.y;
	if (x < 0 || x > 1920 || y < 0 || y > 1080) {
		return false;
	}
	return true;
}

void CarTracker::Render(CanvasWrapper canvas)
{
	// defines colors in RGBA 0-255
	LinearColor colors;
	colors.R = 255;
	colors.G = 255;
	colors.B = 0;
	colors.A = 255;
	canvas.SetColor(colors);

	// sets position to top left
	// x moves to the right
	// y moves down
	// bottom right would be 1920, 1080 for 1080p monitors
	canvas.SetPosition(Vector2F{ 0.0, 0.0 });

	// says hi
	// draws from the last set position
	// the two floats are text x and y scale
	// the false turns off the drop shadow
	canvas.DrawString("Hi Cool Dude", 2.0, 2.0, false);
	Vector2F p1 = Vector2F(100, 200);
	Vector2F p2 = Vector2F(200, 300);
	Vector2F p3 = Vector2F(200, 200);
	
	for (auto& point : points) {
		CVarWrapper cvar_x = cvarManager->getCvar("ct_coord_" + point + "_x");
		if (!cvar_x) { return; }
		float x = cvar_x.getFloatValue();

		CVarWrapper cvar_y = cvarManager->getCvar("ct_coord_" + point + "_y");
		if (!cvar_y) { return; }
		float y = cvar_y.getFloatValue();

		if (point == "p1") {
			p1 = Vector2F(x, y);
		}
		else if (point == "p2") {
			p2 = Vector2F(x, y);
		}
		else if (point == "p3") {
			p3 = Vector2F(x, y);
		}

	}
	canvas.FillTriangle(p1, p2, p3);
}