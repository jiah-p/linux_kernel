int main(void){
    int count = 3;
    while(count--){
        int pid = fork();

        if(pid < 0)
            printf("error\n");
        else if (pid == 0){
            printf("child fork id is %d", pid);
        }
        else{
            printf("child fork id is %d\n", pid);
            printf("parent: %d\n", count);
        }
    }

    return 0;
}