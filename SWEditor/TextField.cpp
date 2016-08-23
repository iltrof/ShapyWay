#include "TextField.h"
#include "Util.h"

TextField::TextField()
{

}

TextField::TextField(int x, int y, int width, int height, sf::Font* font, bool numOnly, bool multiline)
{
	field.setPosition(x, y);
	field.setSize(sf::Vector2f(width, height));
	field.setOutlineColor(sf::Color::White);
	field.setOutlineThickness(1);

	text = "";
	active = false;
	curPos = 0;
	cursorClock.restart();

	this->numOnly = numOnly;
	this->multiline = multiline;
	this->font = font;

	minVal = 0;
	maxVal = 65535;
}

void TextField::input(sf::Event& e)
{
	if(e.type == sf::Event::TextEntered && active)
	{
		if((!numOnly && e.text.unicode >= 32 && e.text.unicode <= 127) || (numOnly && e.text.unicode >= '0' && e.text.unicode <= '9'))
		{
			if(excludedChars.find_first_of(e.text.unicode) == excludedChars.npos)
			{
				text = text.substr(0, curPos)+(char)e.text.unicode+text.substr(std::min(curPos, (int)text.length()));
				curPos++;
			}
		}
		if(e.text.unicode==8)
		{
			text = text.substr(0, std::max(curPos-1, 0))+text.substr(curPos);
			curPos -= (curPos==0) ? 0 : 1;
		}
		else if(e.text.unicode==13 && multiline)
		{
			text = text.substr(0, curPos)+"\n"+text.substr(std::min(curPos, (int)text.length()));
			curPos++;
		}
		cursorClock.restart();
	}
	else if(e.type == sf::Event::KeyPressed && active)
	{
		if(e.key.code == sf::Keyboard::Delete)
		{
			text = text.substr(0, curPos)+text.substr(std::min(curPos+1, (int)text.length()));
		}
		else if(e.key.code == sf::Keyboard::Left)
		{
			curPos -= (curPos==0) ? 0 : 1;
		}
		else if(e.key.code == sf::Keyboard::Right)
		{
			curPos += (curPos==text.length()) ? 0 : 1;
		}
		else if(e.key.code == sf::Keyboard::Up && multiline && curPos!=0 && text.rfind("\n", curPos-1)!=text.npos)
		{
			auto plst = text.rfind("\n", text.rfind("\n", curPos-1)-1);
			int pls; if(plst==text.npos) pls = -1; else pls = plst;
			int nCurPos = pls+curPos-text.rfind("\n", curPos-1);
			if(text.rfind("\n", curPos-1)==text.rfind("\n", nCurPos-1))
				nCurPos = text.rfind("\n", curPos-1);
			curPos = nCurPos;
		}
		else if(e.key.code == sf::Keyboard::Down && multiline && text.find("\n", curPos)!=text.npos)
		{
			int nls; nls = text.find("\n", curPos); int tcl; if(text.rfind("\n", curPos-1)!=text.npos&&curPos!=0) tcl = curPos-text.rfind("\n", curPos-1); else tcl = curPos+1;
			int nCurPos = nls+tcl;
			if(nCurPos>text.length()) nCurPos = text.length();
			if(text.find("\n", curPos)!=text.rfind("\n", nCurPos-1))
				nCurPos = text.rfind("\n", nCurPos-1);
			curPos = nCurPos;
		}
		else if(e.key.code == sf::Keyboard::End)
		{
			if(text.find("\n", curPos)==text.npos) curPos = text.length();
			else curPos = text.find("\n", curPos);
		}
		else if(e.key.code == sf::Keyboard::Home)
		{
			if(text.rfind("\n", curPos-1)==text.npos) curPos = 0;
			else curPos = text.rfind("\n", curPos-1)+1;
		}
		cursorClock.restart();
	}
	else if(e.type == sf::Event::MouseButtonPressed)
	{
		int x = field.getPosition().x, y = field.getPosition().y, sx = field.getSize().x, sy = field.getSize().y;
		if(e.mouseButton.x >= x && e.mouseButton.x < x+sx && e.mouseButton.y >= y && e.mouseButton.y < y+sy)
		{
			curPos = text.length();
			int curDist, prevDist;
			for(int i = 0; i<text.length(); i++)
			{
				sf::Text testText1(text.substr(0, i), *font, 14);
				testText1.setPosition(x+3, y+3);
				sf::Text testText2(text.substr(0, i+1), *font, 14);
				testText2.setPosition(x+3, y+3);

				curDist = abs(e.mouseButton.x - testText2.getGlobalBounds().left - testText2.getLocalBounds().width);
				prevDist = abs(e.mouseButton.x - testText1.getGlobalBounds().left - testText1.getLocalBounds().width);

				if(prevDist < curDist)
				{
					curPos = i;
					break;
				}
			}

			active = true;
			cursorClock.restart();
		}
		else
		{
			active = false;
			if(numOnly)
			{
				int num = 0;
				if(text.length()>0) num = Util::clamp(std::stoi(text), minVal, maxVal);

				text = std::to_string(num);
				if(curPos > text.length()) curPos = text.length();
			}
		}
	}
	else if(e.type == sf::Event::MouseWheelMoved && active && numOnly)
	{
		int x = field.getPosition().x, y = field.getPosition().y, sx = field.getSize().x, sy = field.getSize().y;
		if(e.mouseWheel.x >= x && e.mouseWheel.x < x+sx && e.mouseWheel.y >= y && e.mouseWheel.y < y+sy)
		{
			int num = 0;
			if(text.length()>0) num = std::stoi(text);

			if(e.mouseWheel.delta > 0) num++;
			else num--;
			num = Util::clamp(num, minVal, maxVal);

			text = std::to_string(num);
			curPos = text.length();
		}
	}
}

void TextField::render(sf::RenderTarget* target)
{
	if(cursorClock.getElapsedTime().asMilliseconds()>999) cursorClock.restart();

	field.setFillColor(sf::Color(16+16*active, 16+16*active, 16+16*active));
	target->draw(field);
	sf::Text txt(text, *font, 14);
	txt.setColor(sf::Color::White);
	txt.setPosition(field.getPosition()+sf::Vector2f(3, 3));
	target->draw(txt);

	sf::RectangleShape cursor;
	cursor.setSize(sf::Vector2f(1, 15));
	cursor.setFillColor((cursorClock.getElapsedTime().asMilliseconds()<501) ? sf::Color::White : sf::Color::Transparent);

	sf::Text tmptxt(text.substr(0, curPos), *font, 14);
	std::string tmpstr = tmptxt.getString().toAnsiString();

	int linebreaks = 0;
	for(int i = curPos; i>=0; i--)
	{
		if(tmpstr[i]=='\n') linebreaks++;
	}
	int cursorY = 3+linebreaks*16+txt.getPosition().y;

	if(linebreaks>0)
		tmptxt.setString(tmpstr.substr(std::min(tmpstr.length()-1, tmpstr.rfind("\n")+1)));
	else
		tmptxt.setString(tmpstr);
	int cursorX = tmptxt.getLocalBounds().width + txt.getPosition().x;

	cursor.setPosition(cursorX, cursorY);
	if(active) target->draw(cursor);
}

void TextField::setBounds(int min, int max)
{
	minVal = min;
	maxVal = max;
}