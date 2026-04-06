
int main(*argc, *argv[]);

int main(*argc, *argv[]){

    //variables:

    // gamestate_client -- local simulation state, updated asynchronously

    // gamestate_server -- global simulation state, synchronized across clients


    // client loop:

    //poll client intents
    //send intents to server

    //update local intents object

    //update gamestate_client based on local intents

    //update display using gamestate_client

    //poll server

    //compare server intents object to client intents object

    //if most recent local (client) intents are included
        //if intents dont match
            //overwrite gamestate_client with copy of gamestate_server
        //if intents match
            //overwrite gamestate_server with copy of gamestate_client
    //if most recent intents are not included
        //server messages not up to date yet, do not change anything

}