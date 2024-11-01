#include "Character.cpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>


int playerScore = 0; // Pontuação

class Player; // Declarando jogador

// Mapa do jogo
class GameMap
{
private:
    std::vector<std::string> map;

public:
    GameMap()
    {
        map = {
            "##################################################",
            "#...............#.....................#..........#",
            "#..######.......#......####...........#..........#",
            "#..#...................#....#....................#",
            "#..#......#####........#....#....................#",
            "#..#......#............#....###..##..............#",
            "#..######.#............#............#............#",
            "#..........####....###.#............#............#",
            "#..................#................#............#",
            "#.....##########...#######......#####............#",
            "#.....#..................#..........#............#",
            "#.....#..................#............############",
            "##....#..................#.......................#",
            "#.......#.................#####..................#",
            "#.......##..###############...#.............#....#",
            "#...........................................#....#",
            "#########....#####...#####..................#....#",
            "#..........##............###....####........#....#",
            "#..........#.............#........##.........#####",
            "#....#######.............#.......................#",
            "#....#...................#.......................#",
            "#....#.....######........#######.................#",
            "#....#.....#..................#..................#",
            "#....#######..................#..................#",
            "#...........#######....#########.....#####.......#",
            "#...............................#.....#..........#",
            "#######..##########.............#.....#..........#",
            "#...............................#.....#..........#",
            "#.....................======..........##########.#",
            "##################################################"};
    }

    bool isWall(int x, int y) const
    {
        if (y < 0 || y >= map.size() || x < 0 || x >= map[0].size())
        {
            return true;
        }
        return map[y][x] == '#';
    }

    bool isGate(int x, int y) const
    {
        return y == map.size() - 2 && map[y][x] == '=';
    }

    void display(const Player &player, const std::vector<class Enemy> &enemies, const std::vector<class Projectile> &projectiles);

    const std::vector<std::string> &getMap() const { return map; }
};

// Character- Inimigo
class Enemy : public Character
{
public:
    Enemy(int startX, int startY) : Character(startX, startY, 60) {}

    void moveTowards(const Player &player, const GameMap &gameMap);
    bool isNear(const Player &player) const;
};
// Projeteis
class Projectile
{
private:
    int x, y;
    int deltaX, deltaY;
    int travelDistance;
    bool active;

public:
    Projectile(int startX, int startY, int dx, int dy)
        : x(startX), y(startY), deltaX(dx), deltaY(dy), travelDistance(0), active(true) {}

    bool isActive() const { return active; }
    int getX() const { return x; }
    int getY() const { return y; }

    void move(const GameMap &gameMap, std::vector<Enemy> &enemies)
    {
        if (!active)
            return;

        x += deltaX;
        y += deltaY;
        travelDistance++;

        if (gameMap.isWall(x, y) || travelDistance >= 10)
        {
            active = false;
            std::cout << "O projétil se dissipou.\n";
            return;
        }

        for (auto &enemy : enemies)
        {
            int distanceX = std::abs(enemy.getX() - x);
            int distanceY = std::abs(enemy.getY() - y);

            if ((distanceX == 1 && distanceY == 0) || (distanceX == 0 && distanceY == 1))
            {
                enemy.takeDamage(25);
                std::cout << "O projétil causou 25 de dano a um inimigo próximo!\n";
                if (!enemy.isAlive())
                {
                    std::cout << "Você derrotou um inimigo!\n";
                    playerScore += 10;
                }
                active = false;
                return;
            }
        }
    }
};
// Character- Jogador
class Player : public Character
{
private:
    int score;
    int shootCooldown;
    int teleportCooldown;

public:
    Player(int startX, int startY) : Character(startX, startY, 70), score(0), shootCooldown(0), teleportCooldown(0) {}

    int getHealth() const { return health; }

    void move(char direction, const GameMap &gameMap)
    {
        int newX = x;
        int newY = y;
        switch (direction)
        {
        case '1':
            newX--;
            newY++;
            break;
        case '2':
            newY++;
            break;
        case '3':
            newX++;
            newY++;
            break;
        case '4':
            newX--;
            break;
        case '6':
            newX++;
            break;
        case '7':
            newX--;
            newY--;
            break;
        case '8':
            newY--;
            break;
        case '9':
            newX++;
            newY--;
            break;
        }
        if (!gameMap.isWall(newX, newY))
        {
            x = newX;
            y = newY;
        }
    }

    bool canShoot() const { return shootCooldown == 0; }
    bool canTeleport() const { return teleportCooldown == 0; }

    int getShootCooldown() const { return shootCooldown; }
    int getTeleportCooldown() const { return teleportCooldown; }

    void reduceCooldowns()
    {
        if (shootCooldown > 0)
            shootCooldown--;
        if (teleportCooldown > 0)
            teleportCooldown--;
    }

    Projectile shoot(char direction)
    {
        if (canShoot())
        {
            int dx = 0, dy = 0;

            switch (direction)
            {
            case '1':
                dx = -1;
                dy = 1;
                break;
            case '2':
                dx = 0;
                dy = 1;
                break;
            case '3':
                dx = 1;
                dy = 1;
                break;
            case '4':
                dx = -1;
                dy = 0;
                break;
            case '6':
                dx = 1;
                dy = 0;
                break;
            case '7':
                dx = -1;
                dy = -1;
                break;
            case '8':
                dx = 0;
                dy = -1;
                break;
            case '9':
                dx = 1;
                dy = -1;
                break;
            }

            shootCooldown = 2;
            return Projectile(x, y, dx, dy);
        }
        else
        {
            std::cout << "O jogador precisa esperar " << shootCooldown << " turno(s) para atirar novamente.\n";
            return Projectile(-1, -1, 0, 0);
        }
    }

    void teleport(int distance, char direction, const GameMap &gameMap)
    {
        if (canTeleport())
        {
            if (distance > 9)
            {
                std::cout << "Alcance máximo para teletransporte é 9. Ajustando para 9.\n";
                distance = 9;
            }

            int newX = x;
            int newY = y;

            switch (direction)
            {
            case '1':
                newX -= distance;
                newY += distance;
                break;
            case '2':
                newY += distance;
                break;
            case '3':
                newX += distance;
                newY += distance;
                break;
            case '4':
                newX -= distance;
                break;
            case '6':
                newX += distance;
                break;
            case '7':
                newX -= distance;
                newY -= distance;
                break;
            case '8':
                newY -= distance;
                break;
            case '9':
                newX += distance;
                newY -= distance;
                break;
            }

            if (!gameMap.isWall(newX, newY) && newX >= 0 && newY >= 0 && newY < gameMap.getMap().size() && newX < gameMap.getMap()[0].size())
            {
                x = newX;
                y = newY;
                teleportCooldown = 10;
                std::cout << "Você se teletransportou para (" << x << ", " << y << ").\n";
            }
            else
            {
                std::cout << "Teletransporte falhou! Posição alvo é uma parede ou fora do mapa.\n";
            }
        }
        else
        {
            std::cout << "O jogador precisa esperar " << teleportCooldown << " turno(s) para teletransportar novamente.\n";
        }
    }

   

};

// Metodos dos inimigos
// Movimentação
void Enemy::moveTowards(const Player &player, const GameMap &gameMap)
{
    int deltaX = player.getX() - x;
    int deltaY = player.getY() - y;

    int nextX = x + (deltaX > 0 ? 1 : (deltaX < 0 ? -1 : 0));
    int nextY = y + (deltaY > 0 ? 1 : (deltaY < 0 ? -1 : 0));

    if (!gameMap.isWall(nextX, nextY))
    {
        x = nextX;
        y = nextY;
    }
}
// Calculo de proximidade
bool Enemy::isNear(const Player &player) const
{
    int distanceX = std::abs(player.getX() - x);
    int distanceY = std::abs(player.getY() - y);
    return distanceX <= 1 && distanceY <= 1;
}

// Tela de jogo
void GameMap::display(const Player &player, const std::vector<Enemy> &enemies, const std::vector<Projectile> &projectiles)
{
    for (int i = 0; i < map.size(); i++)
    {
        for (int j = 0; j < map[i].size(); j++)
        {
            bool entityPrinted = false;

            for (const auto &proj : projectiles)
            {
                if (proj.getX() == j && proj.getY() == i && proj.isActive())
                {
                    std::cout << '*';
                    entityPrinted = true;
                    break;
                }
            }
            if (entityPrinted)
                continue;

            for (const auto &enemy : enemies)
            {
                if (enemy.getX() == j && enemy.getY() == i)
                {
                    std::cout << 'E';
                    entityPrinted = true;
                    break;
                }
            }
            if (entityPrinted)
                continue;

            if (i == player.getY() && j == player.getX())
            {
                std::cout << '@';
            }
            else
            {
                std::cout << map[i][j];
            }
        }
        std::cout << std::endl;
    }
    std::cout << "\n";
}
// Introdução do jogo
void displayIntroduction()
{
    std::cout << "Bem-vindo ao Jogo Wandering Wizard\n";
    std::cout << "Você é um feiticeiro viajante e foi contratado pelo rei para destruir ameaças fora do castelo\n";
    std::cout << "Objetivo: Derrote inimigos para ganhar pontos e alcançar o portão do castelo ('=') na parte inferior do mapa.\n";
    std::cout << "Para atravessar o portão, você precisa ter pelo menos 30 pontos.\n\n";
    std::cout << "Comandos básicos:\n";
    std::cout << "1-9 : Mover em direções específicas (no estilo teclado numérico)\n";
    std::cout << "S   : Atirar um projétil\n";
    std::cout << "    - Após pressionar 'S', escolha uma direção (1-9) para o projétil\n";
    std::cout << "    - Direção '5' realiza um ataque corpo a corpo\n";
    std::cout << "T   : Teletransportar\n";
    std::cout << "    - Após pressionar 'T', escolha a distância e uma direção (1-9)\n\n";
    std::cout << "Dica: Fique atento à sua pontuação e vida para conseguir vencer o jogo!\n";
    std::cout << "Pressione Enter para começar o jogo...\n";
    std::cin.ignore();
}

// Metodo principal de lógica do jogo.
int main()
{
    srand(time(0));

    GameMap gameMap;
    Player player(1, 1);

    std::vector<Enemy> enemies;
    int numEnemies = 7;

    for (int i = 0; i < numEnemies; ++i)
    {
        int ex, ey;
        do
        {
            ex = rand() % 50;
            ey = rand() % 30;
        } while (gameMap.isWall(ex, ey) || (ex == player.getX() && ey == player.getY()));

        enemies.emplace_back(ex, ey);
    }

    std::vector<Projectile> projectiles;

    displayIntroduction();

    while (player.isAlive())
    {
        gameMap.display(player, enemies, projectiles);

        if (gameMap.isGate(player.getX(), player.getY()))
        {
            if (playerScore >= 30)
            {
                std::cout << "Parabéns! Você alcançou o portão do castelo e venceu o jogo!" << std::endl;
                break;
            }
            else
            {
                std::cout << "Você precisa de pelo menos 30 pontos para atravessar o portão!\n";
            }
        }
        std::cout << "Vida do Jogador: " << player.getHealth() << "\n";
        std::cout << "Pontos: " << playerScore << "\n";
        std::cout << "\nSelecione um comando:\n";
        std::cout << "1-9 para mover\n";
        std::cout << "S para atirar";
        if (player.getShootCooldown() > 0)
        {
            std::cout << " (" << player.getShootCooldown() << " turno(s) de recarga)";
        }
        std::cout << "\nT para teletransporte";
        if (player.getTeleportCooldown() > 0)
        {
            std::cout << " (" << player.getTeleportCooldown() << " turno(s) de recarga)";
        }
        std::cout << "\n\nEscolha seu comando: ";

        char input;
        std::cin >> input;

        bool validTurn = false;

        if (input == 'S')
        {
            char direction;
            std::cout << "Escolha a direção do disparo (1-9) ou 5 para ataque corpo a corpo: ";
            std::cin >> direction;

            if (direction == '5')
            {
                bool enemyHit = false;
                for (auto &enemy : enemies)
                {
                    if (enemy.getX() == player.getX() && enemy.getY() == player.getY())
                    {
                        enemy.takeDamage(60);
                        std::cout << "Você causou dano extremo a um inimigo na mesma casa!\n";
                        if (!enemy.isAlive())
                        {
                            std::cout << "Você derrotou um inimigo!\n";
                            playerScore += 10;
                        }
                        enemyHit = true;
                        validTurn = true;
                    }
                }
                if (!enemyHit)
                {
                    std::cout << "Não há inimigos na mesma casa para atacar.\n";
                }
            }
            else if (direction >= '1' && direction <= '9')
            {
                if (player.canShoot())
                {
                    Projectile newProjectile = player.shoot(direction);
                    if (newProjectile.isActive())
                    {
                        projectiles.push_back(newProjectile);
                        validTurn = true;
                    }
                }
                else
                {
                    std::cout << "Aguarde " << player.getShootCooldown() << " turno(s) para atirar novamente.\n";
                }
            }
            else
            {
                std::cout << "Direção inválida. Tente novamente.\n";
            }
        }
        else if (input == 'T')
        {
            if (player.canTeleport())
            {
                int distance;
                char direction;
                std::cout << "Escolha a quantidade de casas para teletransportar: ";
                std::cin >> distance;
                std::cout << "Escolha a direção do teletransporte (1-9): ";
                std::cin >> direction;

                player.teleport(distance, direction, gameMap);
                validTurn = true;
            }
            else
            {
                std::cout << "Aguarde " << player.getTeleportCooldown() << " turno(s) para teletransportar novamente.\n";
            }
        }
        else if (input >= '1' && input <= '9')
        {
            player.move(input, gameMap);
            validTurn = true;
        }
        else
        {
            std::cout << "Comando inválido. Tente novamente.\n";
        }

        if (validTurn)
        {
            player.reduceCooldowns();

            for (auto &enemy : enemies)
            {
                enemy.moveTowards(player, gameMap);
            }
            enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                         [](const Enemy &enemy)
                                         { return !enemy.isAlive(); }),
                          enemies.end());

            for (auto &proj : projectiles)
            {
                proj.move(gameMap, enemies);
            }
            projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
                                             [](const Projectile &proj)
                                             { return !proj.isActive(); }),
                              projectiles.end());

            for (auto &enemy : enemies)
            {
                if (enemy.isNear(player))
                {
                    std::cout << "Um inimigo próximo causa dano ao jogador!\n";
                    player.takeDamage(20);
                }
            }
        }
    }

    std::cout << "Jogo terminado! Pontuação final: " << playerScore << std::endl;
    return 0;
}
