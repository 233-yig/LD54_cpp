#pragma once
#include <array>
#include <mine_sweeper.h>

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>

VARIANT_ENUM_CAST(OpResult);
namespace godot{
    class GameBoard: public Control
    {
        GDCLASS(GameBoard, Control)
        MineSweeper game;
        Variant tiles;
        bool dbg;

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
                return static_cast<Array>(tiles)[dbg?11:9];
            case 'w':
                return static_cast<Array>(tiles)[dbg?12:9];
            case 'm':
                return static_cast<Array>(tiles)[dbg?13:9];
            case 'b':
                return static_cast<Array>(tiles)[14];
            }
            return nullptr;
        }

    protected:
        static void _bind_methods()
        {
            BIND_ENUM_CONSTANT(OpResult_Invalid);
            BIND_ENUM_CONSTANT(OpResult_Success);
            BIND_ENUM_CONSTANT(OpResult_Lose);
            BIND_ENUM_CONSTANT(OpResult_Win);
            BIND_ENUM_CONSTANT(OpResult_Count);
            ClassDB::bind_method(D_METHOD("load_data", "map_size", "mine_count", "max_flipped", "map"), &GameBoard::Load);
            ClassDB::bind_method(D_METHOD("flag", "pos"), &GameBoard::Flag);
            ClassDB::bind_method(D_METHOD("flip", "pos"), &GameBoard::Flip);
            ClassDB::bind_method(D_METHOD("revert", "pos"), &GameBoard::Revert);
            ClassDB::bind_method(D_METHOD("debug", "open"), &GameBoard::Debug);
            ClassDB::bind_method(D_METHOD("flip_count"), &GameBoard::FlipCount);
            ClassDB::bind_method(D_METHOD("flag_count"), &GameBoard::FlagCount);
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
                String("{0}/{1}:{2}").format(Array::make((int)GDEXTENSION_VARIANT_TYPE_OBJECT, (int)PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"))
            ));
        }
    public:
        void Debug(bool open)
        {
            dbg = open;
            queue_redraw();
        }
        bool Load(Vector2i mapSize, int mines, int max_flipped, String map)
        {
            bool result = game.Load(mapSize.width, mapSize.height, mines, max_flipped, map.ascii().get_data());
            queue_redraw();
            return result;
        }
        int Flag(Vector2i pos)
        {
            int result = game.Flag(pos.x + pos.y * game.Width());
            if (result == OpResult_Success)
            {
                game.Analyse();
            }
            if(result == OpResult_Success || result == OpResult_Win || result == OpResult_Lose)
            {
                queue_redraw();
            }
            return result;
        }
        int Flip(Vector2i pos)
        {
            int result = game.Flip(pos.x + pos.y * game.Width());
            if (result == OpResult_Success)
            {
                game.Analyse();
            }
            if (result == OpResult_Success || result == OpResult_Win || result == OpResult_Lose)
            {
                queue_redraw();
            }
            return result;
        }
        int Revert(Vector2i pos)
        {
            int result = game.Revert(pos.x + pos.y*game.Width());
            if (result == OpResult_Success)
            {
                game.Analyse();
            }
            if (result == OpResult_Success || result == OpResult_Win || result == OpResult_Lose)
            {
                queue_redraw();
            }
            return result;
        }
        int FlipCount() const
        {
            return game.FlipCount();
        }
        int FlagCount() const
        {
            return game.FlagCount();
        }
        void _draw() override
        {
            for(int i = 0; i < game.Height(); i++)
            {
                for(int j = 0; j < game.Width(); j++)
                {
                    Ref<Texture2D> texture = GetTexture(game.GetState(i * game.Width()+ j));
                    Vector2 texture_size = texture->get_size();
                    draw_texture_rect(texture, Rect2(j * texture_size.x, i * texture_size.y, texture_size.x, texture_size.y), false);
                }
            }
        }
    };
}