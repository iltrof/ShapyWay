#include <SFML/Graphics.hpp>
#include <direct.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <map>
#include <bitset>

#include "Util.h"
#include "Block.h"
#include "BlockDataHandler.h"
#include "Button.h"
#include "WorldRenderer.h"
#include "Action.h"
#include "VersionControl.h"

void fillBlock(int cellx, int celly, int fillID, int fillData, int replaceID, int replaceData);
void loadTextures();
void initBDHandlers();
void saveMap(std::string name);
void loadMap(std::string name);

std::string programDirectory;

sf::Texture* textures; //The array of all textures
sf::Font dFont; //The default font

ColorSelector* colorSelector; //The global color selector
std::vector<BlockDataHandler> bdHandlers; //Handlers for blocks data

int tab = 0; //Current tab
const int numTabs = 5; //Number of tabs
std::string tabsNames[numTabs] = { "Blk", "Wld", "Lnk", "Act", "Hlp"}; //Tabs' labels
enum TABS { TAB_BLOCK, TAB_WORLD, TAB_LINKS, TAB_ACTIONS, TAB_HELP }; //Enum for tabs' IDs

const std::string helpString = //Text shown on the "Hlp" tab
"LMB - Place blocks\n" \
"RMB - Remove blocks\n" \
"F+LMB - Fill with blocks\n" \
"F+RMB - Fill with air\n" \
"P+LMB - Pick a block\n" \
"Ctrl+S - Save map\n" \
"Ctrl+L - Load map\n" \
"Ctrl+F - Focus on screen\n" \
"Space+Mouse - Scroll view\n";

const std::string linksHelpString = //Text shown on the "Lnk" tab
"LMB - Place green links\n" \
"RMB - Place red links\n" \
"Esc - Cancel starting point\n" \
"Click list to select links\n" \
"R - Remove selected link\n" \
"S - Switch link's color\n" \
"\n" \
"Green links are activated\n" \
"when the power turns on\n" \
"Red links are activated\n" \
"when the power goes off\n";

std::vector<Action*> undos;
std::vector<Action*> redos;

void fillBlock(int cellx, int celly, int fillID, int fillData, int replaceID, int replaceData)
{
	if(replaceID == -1) replaceID = blocks[celly*worldWidth*GRID_WIDTH+cellx].ID;
	if(replaceData == -1) replaceData = blocks[celly*worldWidth*GRID_WIDTH+cellx].data;
	if(replaceID == fillID && replaceData == fillData) return;

	setBlock(cellx, celly, fillID, fillData);
	undos.back()->blockChange.blocksBefore.push_back(Block(replaceID, replaceData));
	undos.back()->blockChange.blocksAfter.push_back(Block(fillID, fillData));
	undos.back()->blockChange.positions.push_back(vec2i(cellx, celly));

	if(cellx>0 && Util::getBlockID(blocks, worldWidth, worldHeight, cellx-1, celly) == replaceID 
	&& Util::getBlockData(blocks, worldWidth, worldHeight, cellx-1, celly) == replaceData)
		fillBlock(cellx-1, celly, fillID, fillData, replaceID, replaceData);
	if(cellx<worldWidth*GRID_WIDTH-1 && Util::getBlockID(blocks, worldWidth, worldHeight, cellx+1, celly) == replaceID 
	&& Util::getBlockData(blocks, worldWidth, worldHeight, cellx+1, celly) == replaceData)
		fillBlock(cellx+1, celly, fillID, fillData, replaceID, replaceData);
	if(celly>0 && Util::getBlockID(blocks, worldWidth, worldHeight, cellx, celly-1) == replaceID 
	&& Util::getBlockData(blocks, worldWidth, worldHeight, cellx, celly-1) == replaceData)
		fillBlock(cellx, celly-1, fillID, fillData, replaceID, replaceData);
	if(celly<worldHeight*GRID_HEIGHT-1 && Util::getBlockID(blocks, worldWidth, worldHeight, cellx, celly+1) == replaceID 
	&& Util::getBlockData(blocks, worldWidth, worldHeight, cellx, celly+1) == replaceData)
		fillBlock(cellx, celly+1, fillID, fillData, replaceID, replaceData);
}

void loadTextures()
{
	textures = new sf::Texture[NUM_IDS];

	textures[ID_GRASS].loadFromFile(programDirectory+"res/graphics/tiles/grass.png");
	textures[ID_LASER].loadFromFile(programDirectory+"res/graphics/tiles/laser.png");
	textures[ID_LEVER].loadFromFile(programDirectory+"res/graphics/tiles/lever.png");
	textures[ID_WATER].loadFromFile(programDirectory+"res/graphics/tiles/water.png");
	textures[ID_LAVA].loadFromFile(programDirectory+"res/graphics/tiles/lava.png");
	textures[ID_ACID].loadFromFile(programDirectory+"res/graphics/tiles/acid.png");
	textures[ID_PRESS].loadFromFile(programDirectory+"res/graphics/tiles/press.png");
	textures[ID_PORTAL].loadFromFile(programDirectory+"res/graphics/tiles/portal.png");
	textures[ID_LIGHT].loadFromFile(programDirectory+"res/graphics/tiles/light.png");
	textures[ID_CHECKPOINT].loadFromFile(programDirectory+"res/graphics/tiles/checkpoint.png");
}

void initBDHandlers()
{
	for(int i = 0; i<NUM_IDS; i++)
		bdHandlers.push_back(BlockDataHandler());

	bdHandlers[0] = BlockDataHandler(0, 0, &dFont, colorSelector);
	bdHandlers[ID_GRASS] = BlockDataHandler(0, 0, &dFont, colorSelector);
	bdHandlers[ID_LASER] = BlockDataHandler(0, 1, &dFont, colorSelector);
	bdHandlers[ID_LASER].addField("Power | 0-1", 0, 1, 2, 1, "0");
	bdHandlers[ID_LASER].addField("OR links | 0-1", 0, 1, 3, 1, "0");
	bdHandlers[ID_LEVER] = BlockDataHandler(1, 0, &dFont, colorSelector);
	bdHandlers[ID_LEVER].addField("Power | 0-1", 0, 1, 8, 1, "0");
	bdHandlers[ID_WATER] = BlockDataHandler(0, 0, &dFont, colorSelector);
	bdHandlers[ID_LAVA] = BlockDataHandler(0, 0, &dFont, colorSelector);
	bdHandlers[ID_ACID] = BlockDataHandler(0, 0, &dFont, colorSelector);
	bdHandlers[ID_PRESS] = BlockDataHandler(1, 0, &dFont, colorSelector);
	bdHandlers[ID_PRESS].addField("Power | 0-1", 0, 1, 8, 1, "0");
	bdHandlers[ID_PORTAL] = BlockDataHandler(1, 0, &dFont, colorSelector);
	bdHandlers[ID_LIGHT] = BlockDataHandler(1, 1, &dFont, colorSelector);
	bdHandlers[ID_LIGHT].addField("Range (in blocks) | 0-15", 0, 15, 10, 4, "5");
	bdHandlers[ID_LIGHT].addField("Power | 0-1", 0, 1, 14, 1, "1");
	bdHandlers[ID_LIGHT].addField("OR links | 0-1", 0, 1, 15, 1, "0");
	bdHandlers[ID_CHECKPOINT] = BlockDataHandler(1, 0, &dFont, colorSelector);
	bdHandlers[ID_CHECKPOINT].addField("Raised | 0-1", 0, 1, 8, 1, "0");
}

void saveMap(std::string name)
{
	std::ofstream file; file.open(programDirectory+"maps/"+name+".txt");
	if(!file.is_open()) return;

	file << CURRENT_VERSION << std::endl;
	file << worldWidth << " " << worldHeight << std::endl;
	int curCount = 0, lastID = blocks[0].ID, lastData = blocks[0].data;
	for(int i = 0; i<worldWidth*worldHeight*GRID_WIDTH*GRID_HEIGHT; i++)
	{
		if(blocks[i].ID == lastID && blocks[i].data == lastData)
			curCount++;
		else
		{
			file << curCount << " " << lastID;
			if(lastData != 0) file << ":" << lastData;
			file << " ";
			lastID = blocks[i].ID;
			lastData = blocks[i].data;
			curCount = 1;
		}
	}
	file << curCount << " " << lastID;
	if(lastData != 0) file << ":" << lastData;
	
	file << std::endl;
	file << spawnPoint.x << " " << spawnPoint.y;

	file << std::endl;
	file << links.size() << std::endl;
	for(unsigned int i = 0; i<links.size(); i++)
		file << links[i].from.x << " " << links[i].from.y << " " << links[i].to.x << " " << links[i].to.y << " " << links[i].onPowerOn << std::endl;

	file.close();
}

void loadMap(std::string name)
{
	std::ifstream file; file.open(programDirectory+"maps/"+name+".txt");
	if(!file.is_open()) return;

	interscreenBlocks.clear();
	std::string mapVersion;
	file >> mapVersion;
	file >> worldWidth >> worldHeight;
	delete[] blocks; blocks = new Block[worldWidth*worldHeight*GRID_WIDTH*GRID_HEIGHT];
	for(int i = 0; i<worldWidth*worldHeight*GRID_WIDTH*GRID_HEIGHT;)
	{
		int cnt; file >> cnt;
		if(cnt == -1) break;
		std::string id; file >> id;
		int curID, curData;
		if(id.find(':')==id.npos)
		{
			curID = atoi(id.c_str());
			curData = 0;
		}
		else
		{
			curID = atoi(id.substr(0, id.find(':')).c_str());
			curData = atoi(id.substr(id.find(':')+1).c_str());
		}
		for(int j = 0; j<cnt; i++, j++)
		{
			updateBlock(mapVersion, curID, curData, blocks[i].ID, blocks[i].data);
			if(Util::isInterscreen(blocks[i].ID))
				interscreenBlocks.push_back(vec2i(i%(worldWidth*GRID_WIDTH), i/(worldWidth*GRID_WIDTH)));
		}
	}

	file >> spawnPoint.x >> spawnPoint.y;

	links.clear();
	int linksSize; file >> linksSize;
	for(int i = 0; i<linksSize; i++)
	{
		Link curLink;
		file >> curLink.from.x >> curLink.from.y >> curLink.to.x >> curLink.to.y >> curLink.onPowerOn;
		links.push_back(curLink);
	}

	file.close();
}

void addUndo(Action* undo)
{
	undos.push_back(undo);
	if(undos.size()>30)
	{
		delete undos.front();
		undos.erase(undos.begin());
	}
	for(unsigned int i = 0; i<redos.size(); i++)
		delete redos[i];
	redos.clear();
}

int main(int argc, char** argv)
{
	char cCurrentPath[FILENAME_MAX];

	if(!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
		return 0;

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
	programDirectory = cCurrentPath;
	programDirectory += "\\";

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH+256, WINDOW_HEIGHT), "Editor", sf::Style::Close | sf::Style::Titlebar);
	window.setFramerateLimit(120); bool inFocus = true;
	
	sf::Texture tsTexture; //Texture containing the tileset
	sf::Texture plTexture; //Texture containing the 8-bit palette
	tsTexture.loadFromFile(programDirectory+"res/graphics/editor/tileset.png");
	plTexture.loadFromFile(programDirectory+"res/graphics/editor/palette.png");
	dFont.loadFromFile(programDirectory+"res/fonts/arial.ttf");

	colorSelector = new ColorSelector(WINDOW_WIDTH+32, 256, &plTexture);
	FourWaySelector* addScreenPad = new FourWaySelector(WINDOW_WIDTH+186, 24, -1, "Add Screen", &dFont);
	FourWaySelector* removeScreenPad = new FourWaySelector(WINDOW_WIDTH+186, 84, -1, "Remove Scr.", &dFont);
	Button* setSpawnButton = new Button(WINDOW_WIDTH+64, 244, 128, 24, "Change Spawn", &dFont);
	Button* overviewButton = new Button(WINDOW_WIDTH+64, 280, 128, 24, "Map Overview", &dFont);
	TextField* worldNameField = new TextField(WINDOW_WIDTH+32, 336, 192, 24, &dFont);
	worldNameField->text = "world"; worldNameField->excludedChars = "\\/:?*\"<>| ";
	
	loadTextures();
	initBDHandlers();

	std::string blockNames[NUM_IDS] = { "Air", "Grass", "Laser", "Lever", "Water", "Press", "Portal", "Light", "Checkpoint" }; //Names of blocks

	WorldRenderer::init();

	int curID = 1; //Currently selected block ID
	int curData = 0; //Currently selected block data

	blocks = new Block[worldWidth*worldHeight*GRID_WIDTH*GRID_HEIGHT];
	bool scrollingView = false; //Flag which indicates if the view is being scrolled
	int scrollMX, scrollMY; //Mouse position at the beginning of scrolling (in px)
	int scrollSOX, scrollSOY; //Drawing offsets at the beginning of scrolling (in cells)
	float scrollDX, scrollDY; //Dragged amount (in px)
	bool drawing = false; //Flag which indicates if blocks are being placed/removed
	bool settingSpawn = false; //Flag which indicates if the spawn point is being set
	bool overviewMode = false; //Flag for the overview mode

	vec2i linkStart(-1, -1); //Starting position of a link (when placing links)
	bool greenLink = false; //Whether the link that's being placed is green or not
	int selectedLink = -1; //Index of the currently selected link
	std::vector<int> linksOnScreen; //Array of indices of links whose ends are visible on the screen

	/*** INITIATE GRID VERTICES ***/
	std::vector<sf::Vertex> grid((GRID_WIDTH+1)*2+(GRID_HEIGHT+1)*2);
	for(int i = 0; i<=GRID_WIDTH; i++)
	{
		grid.push_back(sf::Vertex({ i*24.f, 0.f }, sf::Color(255, 255, 255, 64)));
		grid.push_back(sf::Vertex({ i*24.f, WINDOW_HEIGHT }, sf::Color(255, 255, 255, 64)));
	}
	for(int i = 0; i<=GRID_HEIGHT; i++)
	{
		grid.push_back(sf::Vertex({ 0.f, i*24.f }, sf::Color(255, 255, 255, 64)));
		grid.push_back(sf::Vertex({ WINDOW_WIDTH, i*24.f }, sf::Color(255, 255, 255, 64)));
	}

	while(window.isOpen())
	{
		/*** HANDLE ADDING/REMOVING SCREENS ***/
		if(addScreenPad->value >= 0)
		{
			Action* undo = new Action(ACTTYPE_ADDSCREEN);
			undo->screenAdd.direction = addScreenPad->value;
			addUndo(undo);

			addScreen(addScreenPad->value);
			addScreenPad->value = -1;
		}
		if(removeScreenPad->value >= 0)
		{
			Action* undo = new Action(ACTTYPE_REMOVESCREEN);
			undo->screenRemove.direction = removeScreenPad->value;
			
			removeScreen(removeScreenPad->value, undo);
			addUndo(undo);
			removeScreenPad->value = -1;
		}

		/*** HANDLE EVENTS ***/
		sf::Event e;
		while(window.pollEvent(e))
		{
			if(e.type == sf::Event::Closed)
				window.close();
			else if(e.type == sf::Event::MouseButtonPressed && (e.mouseButton.button == sf::Mouse::Left || e.mouseButton.button == sf::Mouse::Right))
			{
				SetFocus(window.getSystemHandle());
				inFocus = true;

				/*** HANDLE DRAWING ***/
				if(e.mouseButton.x < WINDOW_WIDTH && tab != TAB_LINKS)
				{
					if(settingSpawn)
					{
						Action* undo = new Action(ACTTYPE_SETSPAWN);
						undo->spawn.oldPos = spawnPoint;
						undo->spawn.newPos = vec2i(e.mouseButton.x/24 + xoffset, e.mouseButton.y/24 + yoffset);
						addUndo(undo);

						spawnPoint.x = e.mouseButton.x/24 + xoffset;
						spawnPoint.y = e.mouseButton.y/24 + yoffset;
						settingSpawn = false;
						setSpawnButton->label = "Change Spawn";
						continue;
					}
					else if(sf::Keyboard::isKeyPressed(sf::Keyboard::F))
					{
						curData = bdHandlers[curID].getData();

						Action* undo = new Action(ACTTYPE_CHANGEBLOCKS);
						addUndo(undo);

						if(e.mouseButton.button == sf::Mouse::Left)
							fillBlock(e.mouseButton.x/24 + xoffset, e.mouseButton.y/24 + yoffset, curID, curData, -1, -1);
						else if(e.mouseButton.button == sf::Mouse::Right)
							fillBlock(e.mouseButton.x/24 + xoffset, e.mouseButton.y/24 + yoffset, 0, 0, -1, -1);
						continue;
					}
					else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P) && e.mouseButton.button == sf::Mouse::Left)
					{
						curID = Util::getBlockID(blocks, worldWidth, worldHeight, e.mouseButton.x/24 + xoffset, e.mouseButton.y/24 + yoffset);
						curData = blocks[(e.mouseButton.y/24 + yoffset)*worldWidth*GRID_WIDTH + e.mouseButton.x/24 + xoffset].data;
						bdHandlers[curID].setData(curData);
						continue;
					}

					if(!drawing)
					{
						drawing = true;
						Action* undo = new Action(ACTTYPE_CHANGEBLOCKS);
						addUndo(undo);
						curData = bdHandlers[curID].getData();
					}
				}
				/*** HANDLE ADDING LINKS ***/
				else if(e.mouseButton.x < WINDOW_WIDTH && tab == TAB_LINKS)
				{
					selectedLink = -1;
					int cellx = e.mouseButton.x/24 + xoffset, celly = e.mouseButton.y/24 + yoffset;
					int blID = blocks[celly*worldWidth*GRID_WIDTH+cellx].ID;

					if(linkStart.x == -1 && linkStart.y == -1)
					{
						if(Util::isPowering(blID))
						{
							linkStart.x = e.mouseButton.x/24 + xoffset;
							linkStart.y = e.mouseButton.y/24 + yoffset;
							if(e.mouseButton.button == sf::Mouse::Left)
								greenLink = true;
							else
								greenLink = false;
						}
					}
					else if(cellx != linkStart.x || celly != linkStart.y)
					{
						if(Util::isSwitchable(blID))
						{
							links.push_back(Link(linkStart, vec2i(e.mouseButton.x/24 + xoffset, e.mouseButton.y/24 + yoffset), greenLink));
							Action* undo = new Action(ACTTYPE_ADDLINK);
							undo->link.start = linkStart;
							undo->link.end = vec2i(e.mouseButton.x/24 + xoffset, e.mouseButton.y/24 + yoffset);
							undo->link.green = greenLink;
							addUndo(undo);

							linkStart.x = -1;
							linkStart.y = -1;
						}
					}
				}
				/*** HANDLE TAB-RELATED INPUTS ***/
				else
				{
					if(e.mouseButton.y >= WINDOW_HEIGHT-24)
					{
						for(int i = 0; i<numTabs; i++)
						{
							if(e.mouseButton.x > WINDOW_WIDTH+(256/numTabs)*i && e.mouseButton.x < WINDOW_WIDTH+(256/numTabs)*(i+1))
							{
								tab = i;
								if(tab != TAB_LINKS) selectedLink = -1;
								if(tab != TAB_WORLD) 
								{
									settingSpawn = 0; 
									setSpawnButton->label = "Change Spawn";
								}
								break;
							}
						}
					}
					else if(tab == TAB_BLOCK)
					{
						if(e.mouseButton.x >= WINDOW_WIDTH+32 && e.mouseButton.x < WINDOW_WIDTH+224
						   && e.mouseButton.y >= 24 && e.mouseButton.y < 216) //tileset clicked
						{
							int newTileX = e.mouseButton.x - WINDOW_WIDTH-32; newTileX /= 24;
							int newTileY = e.mouseButton.y - 24; newTileY /= 24;

							int newID = newTileY*8+newTileX;
							if(newID < NUM_IDS) curID = newID;
						}
					}
					else if(tab == TAB_WORLD)
					{
						addScreenPad->input(e);
						removeScreenPad->input(e);
						if(!settingSpawn && setSpawnButton->input(e))
						{
							settingSpawn = true;
							setSpawnButton->label = "Click to set";
						}
						if(overviewButton->input(e))
							overviewMode = true;
					}
					else if(tab == TAB_LINKS)
					{
						selectedLink = -1; linkStart.x = -1; linkStart.y = -1;
						if(e.mouseButton.x >= WINDOW_WIDTH+32 && e.mouseButton.x < WINDOW_WIDTH+224)
						{
							for(unsigned int i = 0; i<linksOnScreen.size(); i++)
							{
								if(e.mouseButton.y >= 280+(int)i*24 && e.mouseButton.y < 304+(int)i*24)
								{
									selectedLink = linksOnScreen[i];
									break;
								}
							}
						}
					}
				}
			}
			else if(e.type == sf::Event::KeyPressed)
			{
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)||sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
				{
					worldNameField->active = false;
					if(e.key.code == sf::Keyboard::S)
					{
						saveMap(worldNameField->text);
					}
					else if(e.key.code == sf::Keyboard::L)
					{
						loadMap(worldNameField->text);
					}
					else if(e.key.code == sf::Keyboard::F)
					{
						xoffset = GRID_WIDTH*((xoffset+GRID_WIDTH/2)/GRID_WIDTH);
						yoffset = GRID_HEIGHT*((yoffset+GRID_HEIGHT/2)/GRID_HEIGHT);
					}
					else if(e.key.code == sf::Keyboard::Z && undos.size())
					{
						undoAction(undos.back());
						redos.insert(redos.begin(), undos.back());
						undos.pop_back();
					}
					else if(e.key.code == sf::Keyboard::Y && redos.size())
					{
						redoAction(redos.front());
						undos.push_back(redos.front());
						redos.erase(redos.begin());
					}
				}
				else if(e.key.code == sf::Keyboard::Space && !scrollingView)
				{
					scrollingView = true;
					scrollMX = sf::Mouse::getPosition(window).x;
					scrollMY = sf::Mouse::getPosition(window).y;
					scrollSOX = xoffset;
					scrollSOY = yoffset;
					scrollDX = 0;
					scrollDY = 0;
				}
				else if(scrollingView && (e.key.code == sf::Keyboard::Left || e.key.code == sf::Keyboard::Right
					|| e.key.code == sf::Keyboard::Up || e.key.code == sf::Keyboard::Down))
				{ 
					int xoff = (e.key.code == sf::Keyboard::Right) - (e.key.code == sf::Keyboard::Left);
					int yoff = (e.key.code == sf::Keyboard::Down) - (e.key.code == sf::Keyboard::Up);
					
					scrollDX += 24*xoff; scrollDY += 24*yoff;
				}
				else if(tab == TAB_LINKS)
				{
					if(e.key.code == sf::Keyboard::Escape)
					{
						if(linkStart.x != -1 && linkStart.y != -1)
						{
							linkStart.x = -1;
							linkStart.y = -1;
						}
					}
					else if(e.key.code == sf::Keyboard::R && selectedLink != -1)
					{
						Action* undo = new Action(ACTTYPE_REMOVELINK);
						undo->link.start = links[selectedLink].from;
						undo->link.end = links[selectedLink].to;
						undo->link.green = links[selectedLink].onPowerOn;
						addUndo(undo);

						links.erase(links.begin()+selectedLink);
						selectedLink = -1;
					}
					else if(e.key.code == sf::Keyboard::S && selectedLink != -1)
					{
						Action* undo = new Action(ACTTYPE_SWITCHLINK);
						undo->link.start = links[selectedLink].from;
						undo->link.end = links[selectedLink].to;
						undo->link.green = links[selectedLink].onPowerOn;
						addUndo(undo);

						links[selectedLink].onPowerOn = !links[selectedLink].onPowerOn;
					}
				}
			}
			else if(e.type == sf::Event::KeyReleased)
			{
				if(e.key.code == sf::Keyboard::Space)
					scrollingView = false;
			}
			else if(e.type == sf::Event::LostFocus)
				inFocus = false;
			else if(e.type == sf::Event::GainedFocus)
				inFocus = true;

			bdHandlers[curID].input(e);
			if(tab == TAB_WORLD)
				worldNameField->input(e);
		}

		if(drawing && !sf::Mouse::isButtonPressed(sf::Mouse::Left) && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
			drawing = false;
		else if(overviewMode && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			overviewMode = false;

		/*** HANDLE SCROLLING THE VIEW ***/
		if(scrollingView)
		{
			scrollDX += (sf::Mouse::getPosition(window).x - scrollMX)/10;
			scrollDY += (sf::Mouse::getPosition(window).y - scrollMY)/10;

			xoffset = int(scrollSOX + scrollDX/24);
			yoffset = int(scrollSOY + scrollDY/24);

			if(xoffset < 0)
			{
				xoffset = 0;
				scrollDX = (xoffset - scrollSOX)*24.f;
			}
			if(xoffset >(worldWidth-1)*GRID_WIDTH)
			{
				xoffset = (worldWidth-1)*GRID_WIDTH;
				scrollDX = (xoffset - scrollSOX)*24.f;
			}
			if(yoffset < 0)
			{
				yoffset = 0;
				scrollDY = (yoffset - scrollSOY)*24.f;
			}
			if(yoffset >(worldHeight-1)*GRID_HEIGHT)
			{
				yoffset = (worldHeight-1)*GRID_HEIGHT;
				scrollDY = (yoffset - scrollSOY)*24.f;
			}
		}
		/*** HANDLE PLACING BLOCKS ***/
		else if(sf::Mouse::isButtonPressed(sf::Mouse::Left)&&drawing)
		{
			int mcellx = Util::clamp(sf::Mouse::getPosition(window).x / 24, 0, GRID_WIDTH-1); mcellx += xoffset;
			int mcelly = Util::clamp(sf::Mouse::getPosition(window).y / 24, 0, GRID_HEIGHT-1); mcelly += yoffset;

			int oldID = blocks[mcelly*worldWidth*GRID_WIDTH+mcellx].ID;
			int oldData = blocks[mcelly*worldWidth*GRID_WIDTH+mcellx].data;
			setBlock(mcellx, mcelly, curID, curData);
			if(oldID != curID || oldData != curData)
			{
				undos.back()->blockChange.blocksBefore.push_back(Block(oldID, oldData));
				undos.back()->blockChange.blocksAfter.push_back(Block(curID, curData));
				undos.back()->blockChange.positions.push_back(vec2i(mcellx, mcelly));
			}
		}
		/*** HANDLE ERASING BLOCKS ***/
		else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)&&drawing)
		{
			int mcellx = Util::clamp(sf::Mouse::getPosition(window).x / 24, 0, GRID_WIDTH-1); mcellx += xoffset;
			int mcelly = Util::clamp(sf::Mouse::getPosition(window).y / 24, 0, GRID_HEIGHT-1); mcelly += yoffset;

			int oldID = blocks[mcelly*worldWidth*GRID_WIDTH+mcellx].ID;
			int oldData = blocks[mcelly*worldWidth*GRID_WIDTH+mcellx].data;
			setBlock(mcellx, mcelly, 0, 0);
			if(oldID != 0 || oldData != 0)
			{
				undos.back()->blockChange.blocksBefore.push_back(Block(oldID, oldData));
				undos.back()->blockChange.blocksAfter.push_back(Block(0, 0));
				undos.back()->blockChange.positions.push_back(vec2i(mcellx, mcelly));
			}
		}
		else
			bdHandlers[curID].update(&window);

		/*** CLEAR WINDOW AND DEPTH LAYERS, DRAW GRID ***/
		window.clear(sf::Color(6, 6, 12));
		if(!overviewMode) window.draw(&grid[0], grid.size(), sf::Lines);
		
		if(overviewMode)
			WorldRenderer::renderOverview(blocks, worldWidth, worldHeight, spawnPoint.x, spawnPoint.y, &window);
		else 
			WorldRenderer::render(blocks, worldWidth, worldHeight, xoffset, yoffset, textures, &window, interscreenBlocks);

		/*** DRAW LINKS ***/
		if(tab == TAB_LINKS)
		{
			if(linkStart.x != -1 && linkStart.y != -1)
			{
				sf::CircleShape node(3); node.setOrigin(4, 4);
				node.setPosition((linkStart.x-xoffset)*24.f+12.f, (linkStart.y-yoffset)*24.f+12.f);
				if(greenLink) node.setFillColor(sf::Color::Green);
				else node.setFillColor(sf::Color::Red);
				node.setOutlineThickness(1);

				window.draw(node);
			}

			for(unsigned int i = 0; i<links.size(); i++)
			{
				Link& lnk = links[i];

				sf::CircleShape node(3); node.setOrigin(4, 4);
				if(i == selectedLink) node.setFillColor(sf::Color(0, 128, 255));
				else if(lnk.onPowerOn) node.setFillColor(sf::Color::Green);
				else node.setFillColor(sf::Color::Red);
				node.setOutlineThickness(1);

				sf::Vertex line[2];

				node.setPosition((lnk.from.x-xoffset)*24.f+12.f, (lnk.from.y-yoffset)*24.f+12.f);
				line[0] = sf::Vertex(node.getPosition(), node.getFillColor());
				window.draw(node);
				node.setPosition((lnk.to.x-xoffset)*24.f+12.f, (lnk.to.y-yoffset)*24.f+12.f);
				line[1] = sf::Vertex(node.getPosition(), node.getFillColor());
				window.draw(node);

				window.draw(&line[0], 2, sf::Lines);
			}
		}

		/*** DRAW THE SPAWN MARKER ***/
		sf::Text spawnMarker("S", dFont, 24);
		spawnMarker.setPosition((spawnPoint.x - xoffset)*24.f+3.f, (spawnPoint.y - yoffset)*24.f-3.f);
		window.draw(spawnMarker);

		/*** DRAW WHITE BORDERS BETWEEN SCREENS ***/
		if(!overviewMode)
		{
			std::vector<sf::Vertex> viewLines;
			viewLines.push_back(sf::Vertex({ 24.f*(GRID_WIDTH-(xoffset%GRID_WIDTH)), 0 }));
			viewLines.push_back(sf::Vertex({ 24.f*(GRID_WIDTH-(xoffset%GRID_WIDTH)), WINDOW_HEIGHT }));
			viewLines.push_back(sf::Vertex({ 0, 24.f*(GRID_HEIGHT-(yoffset%GRID_HEIGHT)) }));
			viewLines.push_back(sf::Vertex({ WINDOW_WIDTH, 24.f*(GRID_HEIGHT-(yoffset%GRID_HEIGHT)) }));
			window.draw(&viewLines[0], viewLines.size(), sf::Lines);
		}

		/*** DRAW THE SIDEBAR ***/
		sf::RectangleShape sidebar(sf::Vector2f(256, WINDOW_HEIGHT));
		sidebar.setFillColor(sf::Color(32, 32, 32));
		sidebar.setPosition(WINDOW_WIDTH, 0);
		window.draw(sidebar);

		/*** DRAW TABS' CONTENTS ***/
		if(tab == TAB_BLOCK)
		{
			sf::Sprite tileset(tsTexture);
			tileset.setPosition(WINDOW_WIDTH+32, 24);
			window.draw(tileset);

			std::vector<sf::Vertex> selectionLines;
			selectionLines.push_back(sf::Vertex({ WINDOW_WIDTH+32, 24.f+24.f*(curID/8) }, sf::Color(255, 255, 255, 96)));
			selectionLines.push_back(sf::Vertex({ WINDOW_WIDTH+224, 24.f+24.f*(curID/8) }, sf::Color(255, 255, 255, 96)));
			selectionLines.push_back(sf::Vertex({ WINDOW_WIDTH+32, 48.f+24.f*(curID/8) }, sf::Color(255, 255, 255, 96)));
			selectionLines.push_back(sf::Vertex({ WINDOW_WIDTH+224, 48.f+24.f*(curID/8) }, sf::Color(255, 255, 255, 96)));
			selectionLines.push_back(sf::Vertex({ WINDOW_WIDTH+32+24.f*(curID%8), 24 }, sf::Color(255, 255, 255, 96)));
			selectionLines.push_back(sf::Vertex({ WINDOW_WIDTH+32+24.f*(curID%8), 216 }, sf::Color(255, 255, 255, 96)));
			selectionLines.push_back(sf::Vertex({ WINDOW_WIDTH+56+24.f*(curID%8), 24 }, sf::Color(255, 255, 255, 96)));
			selectionLines.push_back(sf::Vertex({ WINDOW_WIDTH+56+24.f*(curID%8), 216 }, sf::Color(255, 255, 255, 96)));
			window.draw(&selectionLines[0], selectionLines.size(), sf::Lines);

			sf::Text selectedBlockText("Selected: " + blockNames[curID], dFont, 16);
			selectedBlockText.setPosition(WINDOW_WIDTH+128-selectedBlockText.getLocalBounds().width/2, 224);
			window.draw(selectedBlockText);

			bdHandlers[curID].render(&window);
		}
		else if(tab == TAB_WORLD)
		{
			addScreenPad->render(&window);
			removeScreenPad->render(&window);

			std::string worldInfoStr = "Current world size:\n   " + std::to_string(worldWidth) + ", " + std::to_string(worldHeight);
			sf::Text worldInfoText(worldInfoStr, dFont, 16);
			worldInfoText.setPosition(WINDOW_WIDTH+32, 144);
			window.draw(worldInfoText);

			std::string spawnInfoStr = "Current spawn position:\n   " + std::to_string(spawnPoint.x) + ", " + std::to_string(spawnPoint.y);
			sf::Text spawnInfoText(spawnInfoStr, dFont, 16);
			spawnInfoText.setPosition(WINDOW_WIDTH+32, 192);
			window.draw(spawnInfoText);

			setSpawnButton->render(&window);
			overviewButton->render(&window);

			sf::Text worldNameText("Map name:", dFont, 16);
			worldNameText.setPosition(WINDOW_WIDTH+32, 312);
			window.draw(worldNameText);

			worldNameField->render(&window);
		}
		else if(tab == TAB_LINKS)
		{
			sf::Text helpText(linksHelpString, dFont, 16);
			helpText.setPosition(WINDOW_WIDTH+32, 24);
			window.draw(helpText);

			sf::Text linksListText("Links on the screen:", dFont, 16);
			linksListText.setPosition(WINDOW_WIDTH+32, 256);
			window.draw(linksListText);

			linksOnScreen.clear(); bool clearLinkSelection = true;
			for(unsigned int i = 0; i<links.size(); i++)
			{
				Link& lnk = links[i];
				if((lnk.from.x >= xoffset && lnk.from.x < xoffset+GRID_WIDTH && lnk.from.y >= yoffset && lnk.from.y < yoffset+GRID_HEIGHT)
				   ||(lnk.to.x >= xoffset && lnk.to.x < xoffset+GRID_WIDTH && lnk.to.y >= yoffset && lnk.to.y < yoffset+GRID_HEIGHT))
				{
					linksOnScreen.push_back(i);
					if(i == selectedLink)
						clearLinkSelection = false;
				}
			}
			if(clearLinkSelection) selectedLink = -1;
			for(unsigned int i = 0; i<linksOnScreen.size(); i++)
			{
				Link& lnk = links[linksOnScreen[i]];

				std::string linkInfoString;
				linkInfoString = std::to_string(lnk.from.x) + ", " + std::to_string(lnk.from.y) + " -> "
					+ std::to_string(lnk.to.x) + ", " + std::to_string(lnk.to.y) + " (" + (lnk.onPowerOn ? "1)" : "0)");
				sf::Text linkInfoText(linkInfoString, dFont, 16);
				if(linksOnScreen[i] == selectedLink)
					linkInfoText.setFillColor(sf::Color::Cyan);
				linkInfoText.setPosition({ WINDOW_WIDTH+32, 280.f+i*24.f });
				window.draw(linkInfoText);
			}
		}
		else if(tab == TAB_HELP)
		{
			sf::Text helpText(helpString, dFont, 16);
			helpText.setPosition(WINDOW_WIDTH+32, 24);
			window.draw(helpText);
		}

		/*** DRAW TABS' LABELS ***/
		for(int i = 0; i<numTabs; i++)
		{
			sf::RectangleShape tabRect(sf::Vector2f(256/numTabs - 2, 23));
			tabRect.setOutlineThickness(1);
			tabRect.setFillColor(sf::Color(16+(tab==i)*48, 16+(tab==i)*48, 16+(tab==i)*48));
			tabRect.setPosition({ WINDOW_WIDTH+1+(float)(256/numTabs)*i, WINDOW_HEIGHT-23 });
			window.draw(tabRect);

			sf::Text tabText(tabsNames[i], dFont, 16);
			tabText.setPosition(tabRect.getPosition().x + tabRect.getSize().x/2 - tabText.getLocalBounds().width/2, tabRect.getPosition().y+2);
			window.draw(tabText);
		}

		window.display();
	}
	return 0;
}