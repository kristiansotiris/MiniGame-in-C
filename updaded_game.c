#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t is_dead = PTHREAD_COND_INITIALIZER;
int game_over = 0;

typedef struct {
    char Category[30];
    char Name[30];
    int player_hp;
    int player_attack_dmg;
} Player;

typedef struct {
    char Category[30];
    char Name[30];
    int enemy_hp;
    int enemy_attack_dmg;
} Enemy_Player;

Enemy_Player *enemies = NULL;
int num_of_enemies = 0;

Player *players = NULL;
int num_of_players = 0;

int is_dead_check(int hp) {
    return hp <= 0;
}

void *player_thread(void *arg) {
    Player *player = (Player *)arg;

    while (!game_over && !is_dead_check(player->player_hp)) {
        sleep(3);
        pthread_mutex_lock(&mutex);

        // Player attacks all enemies
        for (int i = 0; i < num_of_enemies; i++) {
            if (!is_dead_check(enemies[i].enemy_hp)) {
                enemies[i].enemy_hp -= player->player_attack_dmg;
                printf("%s Attacks %s! Enemy HP: %d\n", player->Name, enemies[i].Name, enemies[i].enemy_hp);
            }

            if (is_dead_check(enemies[i].enemy_hp)) {
                game_over = 1;
                printf("%s Is Being Defeated By %s!\n", enemies[i].Name, player->Name);
                pthread_cond_broadcast(&is_dead); // Notify game over
                break;
            }
        }

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void *enemy_thread(void* arg) {
    Player *players = (Player *)arg;

    while (!game_over) {
        sleep(2);

        pthread_mutex_lock(&mutex);

        if (enemies[0].enemy_hp > 0) {  // If enemy is still alive
            int all_players_are_dead = 1;

            // Enemy attacks all players
            for (int i = 0; i < num_of_players; i++) {
                if (!is_dead_check(players[i].player_hp)) {
                    players[i].player_hp -= enemies[0].enemy_attack_dmg;
                    printf("%s Attacks %s! Player HP: %d\n", enemies[0].Name, players[i].Name, players[i].player_hp);
                }

                if (is_dead_check(players[i].player_hp)) {
                    printf("%s Player Has Been Defeated!\n", players[i].Name);
                } else {
                    all_players_are_dead = 0;
                }
            }

            // If all players are dead, end the game
            if (all_players_are_dead) {
                printf("All players have been defeated!\n");
                game_over = 1;
                pthread_cond_broadcast(&is_dead);
            }
        } else {
            // If enemy is dead, end the game
            game_over = 1;
            pthread_cond_broadcast(&is_dead);
        }

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}


int main() {
    // Create players dynamically (you can adjust the number of players here)
    num_of_players = 2;
    players = (Player *)malloc(num_of_players * sizeof(Player));

    // Assign player data
    players[0] = (Player){"Fighter", "Arthur of Demasia", 180, 40};
    players[1] = (Player){"Archer", "Evelyn of the Woods", 150, 40};

    // Create enemies dynamically (you can adjust the number of enemies here)
    num_of_enemies = 1;
    enemies = (Enemy_Player *)malloc(num_of_enemies * sizeof(Enemy_Player));

    // Assign enemy data
    enemies[0] = (Enemy_Player){"Dragon", "Fire Drake", 200, 40};

    pthread_t *player_threads = (pthread_t *)malloc(num_of_players * sizeof(pthread_t));
    pthread_t *enemy_threads = (pthread_t *)malloc(num_of_enemies * sizeof(pthread_t));

    // Create player threads
    for (int i = 0; i < num_of_players; i++) {
        if (pthread_create(&player_threads[i], NULL, player_thread, (void *)&players[i]) != 0) {
            perror("Failed to create player thread");
            exit(1);
        }
    }

    // Create enemy threads
    for (int i = 0; i < num_of_enemies; i++) {
        if (pthread_create(&enemy_threads[i], NULL, enemy_thread, (void *)players) != 0) {
            perror("Failed to create enemy thread");
            exit(1);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_of_players; i++) {
        pthread_join(player_threads[i], NULL);
    }

    for (int i = 0; i < num_of_enemies; i++) {
        pthread_join(enemy_threads[i], NULL);
    }

    // Clean up resources
    free(players);
    free(enemies);
    free(player_threads);
    free(enemy_threads);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&is_dead);

    return 0;
}
