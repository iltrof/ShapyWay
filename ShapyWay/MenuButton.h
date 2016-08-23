#include <SFML\Graphics.hpp>

class MenuButton
{
public:
	MenuButton();
	MenuButton(std::string label, sf::Font* font, int x, int y, int w, int h);

	bool input(sf::Event& e);
	void update(sf::Vector2i& mousePos);
	void render(sf::RenderTarget* target);
private:
	std::string label;
	sf::Font* font;
	int x, y;
	int sx, sy;

	sf::RectangleShape rect;
};

class RoundMenuButton
{
public:
	RoundMenuButton();
	RoundMenuButton(std::string spritePath, int x, int y, int radius);

	bool input(sf::Event& e);
	void update(sf::Vector2i& mousePos);
	void render(sf::RenderTarget* target);
	bool glowing;
private:
	sf::Texture texture;
	
	int x, y;
	int radius;
};