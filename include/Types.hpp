#pragma once

// перечисление состояний основного цикла
enum class GameState { Menu, Playing, Paused, GameOver, Records };

// типы башен с разным поведением атаки
enum class TowerType { Normal, Cryo, Poison };
