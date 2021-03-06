#pragma once

#include "Core/AppItems/mvTypeBases.h"
#include "mvApp.h"
#include "mvEventHandler.h"

namespace Marvel {

	//-----------------------------------------------------------------------------
	// mvWindowAppitem
	//-----------------------------------------------------------------------------
	class mvWindowAppitem : public mvAppItem, public mvEventHandler
	{

	public:

		MV_APPITEM_TYPE(mvAppItemType::Window)

		mvWindowAppitem(const std::string& parent, const std::string& name, int width, int height, int xpos, int ypos, 
			bool mainWindow, bool autosize, bool resizable, bool titlebar, bool movable, const std::string& closing_callback="")
			: mvAppItem(parent, name), mvEventHandler(), m_xpos(xpos), m_ypos(ypos), m_mainWindow(mainWindow), m_closing_callback(closing_callback)
		{
			m_container = true;
			m_width = width;
			m_height = height;

			if (mainWindow)
			{
				m_windowflags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings
					| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
			}

			if (autosize)
				m_windowflags |= ImGuiWindowFlags_AlwaysAutoResize;

			if(!resizable)
				m_windowflags |= ImGuiWindowFlags_NoResize;

			if (!titlebar)
				m_windowflags |= ImGuiWindowFlags_NoTitleBar;

			if (!movable)
				m_windowflags |= ImGuiWindowFlags_NoMove;
		}

		void addFlag(ImGuiWindowFlags flag) { m_windowflags |= flag; }
		void removeFlag(ImGuiWindowFlags flag) { m_windowflags &= ~flag; }
		
		void setWindowPos(float x, float y)
		{
			m_xpos = x;
			m_ypos = y;
			m_dirty = true;
		}

		virtual void setWidth(int width) override { m_width = width; m_dirty = true; }
		virtual void setHeight(int height) override { m_height = height; m_dirty = true; }

		mvVec2 getWindowPos() const
		{
			return { (float)m_xpos, (float)m_ypos };
		}

		virtual void draw() override
		{
			// shouldn't have to do this but do. Fix later
			if (!m_show)
			{
				setHovered(false);
				setFocused(false);
				setActivated(false);
				setVisible(false);
				if (!m_closing)
				{
					m_closing = true;
					mvApp::GetApp()->runCallback(m_closing_callback, m_name);

				}
				return;
			}
			m_closing = false;

			if (m_mainWindow)
			{
				ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
				ImGui::SetNextWindowSize(ImVec2(m_width, m_height));
			}
				
			else if (m_dirty)
			{
				ImGui::SetNextWindowPos(ImVec2(m_xpos, m_ypos));
				ImGui::SetNextWindowSize(ImVec2(m_width, m_height));
				m_dirty = false;
			}

			if (!ImGui::Begin(m_label.c_str(), &m_show, m_windowflags))
			{
				ImGui::End();
				return;
			}

			for (mvAppItem* item : m_children)
			{
				// skip item if it's not shown
				if (!item->isShown())
					continue;

				// set item width
				if (item->getWidth() > 0)
					ImGui::SetNextItemWidth((float)item->getWidth());

				item->pushColorStyles();
				item->draw();
				item->popColorStyles();

				// Regular Tooltip (simple)
				if (item->getTip() != "" && ImGui::IsItemHovered())
					ImGui::SetTooltip(item->getTip().c_str());

				item->setHovered(ImGui::IsItemHovered());
				item->setActive(ImGui::IsItemActive());
				item->setFocused(ImGui::IsItemFocused());
				item->setClicked(ImGui::IsItemClicked());
				item->setVisible(ImGui::IsItemVisible());
				item->setEdited(ImGui::IsItemEdited());
				item->setActivated(ImGui::IsItemActivated());
				item->setDeactivated(ImGui::IsItemDeactivated());
				item->setDeactivatedAfterEdit(ImGui::IsItemDeactivatedAfterEdit());
				item->setToggledOpen(ImGui::IsItemToggledOpen());
				item->setRectMin({ ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y });
				item->setRectMax({ ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y });
				item->setRectSize({ ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y });

			}

			setVisible(true);
			setHovered(ImGui::IsWindowHovered());
			setFocused(ImGui::IsWindowFocused());
			setRectSize({ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y });
			setActivated(ImGui::IsWindowCollapsed());

			if (ImGui::GetWindowWidth() != m_width || ImGui::GetWindowHeight() != m_height)
				mvApp::GetApp()->runCallback(getResizeCallback(), m_name);

			m_width = ImGui::GetWindowWidth();
			m_height = ImGui::GetWindowHeight();

			if (ImGui::IsWindowFocused())
			{

				float titleBarHeight = ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetFontSize();

				// update mouse
				ImVec2 mousePos = ImGui::GetMousePos();
				mvInput::setGlobalMousePosition(mousePos.x, mousePos.y);
				float x = mousePos.x - ImGui::GetWindowPos().x;
				float y = mousePos.y - ImGui::GetWindowPos().y - titleBarHeight;
				mvInput::setMousePosition(x, y);
				mvApp::GetApp()->setActiveWindow(m_name);

			}

			m_xpos = ImGui::GetWindowPos().x;
			m_ypos = ImGui::GetWindowPos().y;

			ImGui::End();
		}

	private:

		ImGuiWindowFlags m_windowflags = ImGuiWindowFlags_NoSavedSettings;
		int              m_xpos = 200;
		int              m_ypos = 200;
		bool             m_mainWindow = false;
		bool             m_dirty = true;
		bool             m_closing = true;
		std::string      m_closing_callback = "";

	};

}