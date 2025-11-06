#pragma once
#include <vector>
#include <utility>

enum class TileId {
    TILE_0, TILE_1, TILE_2, TILE_3, TILE_4, TILE_5, TILE_6, TILE_7, TILE_8, TILE_9,
    TILE_10, TILE_11, TILE_12, TILE_13, TILE_14, TILE_15, TILE_16, TILE_17, TILE_18, TILE_19,
    TILE_20, TILE_21, TILE_22, TILE_23, TILE_24, TILE_25, TILE_26, TILE_27, TILE_28, TILE_29,
    TILE_30, TILE_31, TILE_32, TILE_33, TILE_34, TILE_35, TILE_36, TILE_37, TILE_38, TILE_39,
    TILE_40, TILE_41, TILE_42, TILE_43, TILE_44, TILE_45, TILE_46, TILE_47, TILE_48, TILE_49,
    TILE_50, TILE_51, TILE_52, TILE_53, TILE_54, TILE_55, TILE_56, TILE_57, TILE_58, TILE_59,
    TILE_60, TILE_61, TILE_62, TILE_63, TILE_64, TILE_65, TILE_66, TILE_67, TILE_68, TILE_69,
    TILE_70, TILE_71, TILE_72, TILE_73, TILE_74, TILE_75, TILE_76, TILE_77, TILE_78, TILE_79,
    TILE_80, TILE_81, TILE_82, TILE_83, TILE_84, TILE_85, TILE_86, TILE_87, TILE_88, TILE_89,
    TILE_90, TILE_91, TILE_92, TILE_93, TILE_94, TILE_95,
    TOTAL_TILES = 96
};

class Tile {
public:
    std::vector<std::pair<int, int>> blocks;
    
    Tile() = default;
    Tile(const std::vector<std::pair<int, int>>& coords) : blocks(coords) {}
    
    static const Tile& getTile(TileId id);
    static int toInt(TileId id) { return static_cast<int>(id); }
    static TileId fromInt(int id) { return static_cast<TileId>(id); }
    
    Tile rotate90() const;
    Tile flipHorizontal() const;
    Tile flipVertical() const;
    int getWidth() const;
    int getHeight() const;
    Tile normalized() const;
    bool isValid() const { return !blocks.empty(); }
    size_t getBlockCount() const { return blocks.size(); }
};
