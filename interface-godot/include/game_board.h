#pragma once
#include <mine_sweeper.h>
#include <format>

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/array.hpp>

VARIANT_ENUM_CAST(OpResult);
namespace godot{
    class GameBoard: public Control
    {
        GDCLASS(GameBoard, Control)

        Variant tiles;
        Ref<Texture2D> GetTexture(char state)
        {
            if(state >= '0' && state <= '8')
            {
                return static_cast<Array>(tiles)[state - '0'];
            }
            switch(state)
            {
            case 'e':
                return static_cast<Array>(tiles)[9];
            case 'f':
                return static_cast<Array>(tiles)[10];
            case 's':
                return static_cast<Array>(tiles)[11];
            case 'w':
                return static_cast<Array>(tiles)[12];
            case 'm':
                return static_cast<Array>(tiles)[13];
            }
            return nullptr;
        }

        MineSweeper game;
    protected:
        static void _bind_methods()
        {
            BIND_ENUM_CONSTANT(OpResult_Invalid);
            BIND_ENUM_CONSTANT(OpResult_Success);
            BIND_ENUM_CONSTANT(OpResult_Lose);
            BIND_ENUM_CONSTANT(OpResult_Win);
            BIND_ENUM_CONSTANT(OpResult_Count);
            ClassDB::bind_method(D_METHOD("load", "width", "height", "mines", "map"), &Load);
            ClassDB::bind_method(D_METHOD("flag", "x", "y"), &Flag);
            ClassDB::bind_method(D_METHOD("flip", "x", "y"), &Flip);
        }
        bool _get(const StringName &p_property, Variant &r_value) const // return true if property was found
        {
            String name = p_property;
            if(name == "tiles")
            {
                r_value = tiles;
                return true;
            }
            return false;
        }
        bool _set(const StringName &p_property, const Variant &p_value) // return true if property was found
        {
            String name = p_property;
            if(name == "tiles")
            {
                tiles = p_value;
                return true;
            }
            return false;
        }
        void _get_property_list(List<PropertyInfo> *r_props) const      // return list of properties
        {
            r_props->push_back(PropertyInfo(
                Variant::ARRAY,"tiles", PROPERTY_HINT_ARRAY_TYPE, 
                std::format("{}/{}:{}", (int)GDEXTENSION_VARIANT_TYPE_OBJECT, (int)PROPERTY_HINT_RESOURCE_TYPE, "Texture2D").c_str()
            ));
        }
    public:
        void Load(int width, int height, int mines, String map)
        {
            game.Load(width, height, mines, map.ascii().get_data());
        }
        int Flag(int x, int y)
        {
            int result = game.Flag(x, y);
            queue_redraw();
            return result;
        }
        int Flip(int x, int y)
        {
            int result = game.Flip(x, y);
            queue_redraw();
            return result;
        }
        void _draw() override
        {
            for(int i = 0; i < game.Height(); i++)
            {
                for(int j = 0; j < game.Width(); j++)
                {
                    Ref<Texture2D> texture = GetTexture(game.GetState(i * game.Width()+ j));
                    Vector2 texture_size = texture->get_size();
                    draw_texture_rect(texture, Rect2((j - 0.5) * texture_size.x, (i - 0.5) * texture_size.y, texture_size.x, texture_size.y), false);
                }
            }
        }

    };
}