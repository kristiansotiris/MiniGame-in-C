#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PLAYER_ATTACK 20
#define ENEMY_ATTACK 30


int player_hp = 100;
int enemy_hp = 100;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * player_thread(void* arg){

    char input;

    while(player_hp > 0 && enemy_hp > 0){
        printf("Press 'A' To Attack to the Enemy (Attack Damage: %d) \n", PLAYER_ATTACK);
        scanf(" %s", &input);

        if(input == 'A' || input == 'a'){
            pthread_mutex_lock(&mutex);

            enemy_hp-=PLAYER_ATTACK;

            printf("Enemy HP After the Attack: %d \n", enemy_hp);

            if(enemy_hp <= 0){
                printf("You won! The enemy is defeated.\n");
                pthread_mutex_unlock(&mutex);
                exit(0);
            }

            pthread_mutex_unlock(&mutex);
        }else{
            printf("Wrong key! PRESS 'A' TO ATTACK! Try again \n");
        }
    }

    return NULL;

}

void * enemy_thread(void* arg){

    while(player_hp > 0 && enemy_hp > 0){
        sleep(2);

        pthread_mutex_lock(&mutex);
        if(player_hp > 0 && enemy_hp > 0){
            player_hp -= ENEMY_ATTACK;
            printf("Enemy Is Attacking... Player HP : %d \n", player_hp);
        }

        if(player_hp <= 0){
            printf("You Lose ! \n");
            pthread_mutex_unlock(&mutex);
            exit(0);
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


int main(){

    pthread_t player, enemy;

    if(pthread_create(&player, NULL, player_thread, NULL) !=0){
        perror("Couldn't Create Player Thread !");
        exit(1);
    }

    if(pthread_create(&enemy, NULL, enemy_thread, NULL) !=0 ){
        perror("Couldn't Create Enemy Thread");
        exit(1);
    }

    pthread_join(player, NULL);
    pthread_join(enemy, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;

  //HOPE YOU LIKE IT AND MAKE SOMETHING COOLER FROM THAT CREATED BY Kristian Sotiri


}
