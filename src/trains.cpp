#include "trains.h"
#include "simulation_state.h"
#include "grid.h"
#include "switches.h"
#include <cstdlib>
#include<iostream>
using namespace std;
// ============================================================================
// TRAINS.CPP - Train logic
// ============================================================================

// ----------------------------------------------------------------------------
// DIRECTION NAME
// ----------------------------------------------------------------------------
const char* directionName(int d) {
if(d==0) return "UP";
if(d==1) return "RIGHT";
if(d==2) return "DOWN";
return "LEFT";
}

// ----------------------------------------------------------------------------
// END A TRAIN (delivered or crashed)
// ----------------------------------------------------------------------------
void endTrain(int k, int state) {
final_state[k]=state;
final_pos[k][0]=active_coordinates[k][0];
final_pos[k][1]=active_coordinates[k][1];
trainActive[k]=false;
active_coordinates[k][0]=-1;
active_coordinates[k][1]=-1;
if(state==1) reach++;
else crash++;
}

// ----------------------------------------------------------------------------
// SPAWN TRAINS FOR CURRENT TICK
// ----------------------------------------------------------------------------
// Activate trains scheduled for this tick.
// If the spawn tile is occupied, the train waits and tries again next tick.
// ----------------------------------------------------------------------------
void spawnTrainsForTick() {
    for(int k=0;k<t_train;k++){
        if(!trainActive[k] && active_coordinates[k][0]!=-1 && tick>=trains[k][0]){
            bool occupied=false;
            for(int u=0;u<t_train;u++){
                if(trainActive[u] && active_coordinates[u][0]==trains[k][1] && active_coordinates[u][1]==trains[k][2])
                occupied=true;
            }
            if(occupied)
            continue;
            trainActive[k]=1;
            active_coordinates[k][0]=trains[k][1];
            active_coordinates[k][1]=trains[k][2];
            active_direc[k]=trains[k][3];
            active_colour[k]=trains[k][4];
            prev_coordinates[k][0]=trains[k][1];
            prev_coordinates[k][1]=trains[k][2];
            cout<<"Train "<<k<<" spawned at ("<<trains[k][1]<<","<<trains[k][2]<<")\n";
            pdist=10000;
            for(int i=0;i<100;i++)
            {
            for(int j=0;j<100;j++)
            {
            if(grid[i][j]=='D'){  
            dist = abs(active_coordinates[k][0] - j) + abs(active_coordinates[k][1] - i);
            if(pdist>dist) {
            pdist=dist;
            fdest[k][0]=j;
            fdest[k][1]=i;
            }
            }
            }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// DETERMINE NEXT POSITION for a train
// ----------------------------------------------------------------------------
// Compute next position/direction from current tile and rules.
// ----------------------------------------------------------------------------
bool determineNextPosition(int k){
if(next_direc[k]==0){
next_coor[k][0]=active_coordinates[k][0];
next_coor[k][1]=active_coordinates[k][1]-1;
}
if(next_direc[k]==1){
next_coor[k][0]=active_coordinates[k][0]+1;
next_coor[k][1]=active_coordinates[k][1];
}
if(next_direc[k]==2){
next_coor[k][0]=active_coordinates[k][0];
next_coor[k][1]=active_coordinates[k][1]+1;
}
if(next_direc[k]==3){
next_coor[k][0]=active_coordinates[k][0]-1;
next_coor[k][1]=active_coordinates[k][1];
}
return 1;
}

// ----------------------------------------------------------------------------
// BEST OPEN DIRECTION (helper)
// ----------------------------------------------------------------------------
// Among straight, right and left (never reverse), keep only the directions whose
// next tile is track, and return the one closest to the train's destination.
// If straight is allowed and ties, straight wins. If nothing is open, go straight.
// ----------------------------------------------------------------------------
static int bestOpenDirection(int k, bool allowStraight) {
int best=active_direc[k];
int bestDist=100000;
int dx[4]={0,1,0,-1};
int dy[4]={-1,0,1,0};
for(int d=0; d<4; d++) {
if(d==(active_direc[k]+2)%4) continue;
if(d==active_direc[k] && !allowStraight) continue;
int nx=active_coordinates[k][0]+dx[d];
int ny=active_coordinates[k][1]+dy[d];
if(!isInBounds(ny,nx) || !isTrackTile(ny,nx)) continue;
int dd=abs(nx-fdest[k][0])+abs(ny-fdest[k][1]);
if(dd<bestDist || (dd==bestDist && d==active_direc[k])) {
bestDist=dd;
best=d;
}
}
return best;
}

// ----------------------------------------------------------------------------
// SIDE OR STRAIGHT (helper)
// ----------------------------------------------------------------------------
// Used by the LEFT and RIGHT switch states. Returns direction d if the tile in
// that direction is track, otherwise the train keeps going straight.
// ----------------------------------------------------------------------------
static int sideOrStraight(int k, int d) {
int dx[4]={0,1,0,-1};
int dy[4]={-1,0,1,0};
int nx=active_coordinates[k][0]+dx[d];
int ny=active_coordinates[k][1]+dy[d];
if(isInBounds(ny,nx) && isTrackTile(ny,nx))
return d;
return active_direc[k];
}

// ----------------------------------------------------------------------------
// GET NEXT DIRECTION based on current tile and direction
// ----------------------------------------------------------------------------
// Return new direction after entering the tile.
// ----------------------------------------------------------------------------
int getNextDirection(int k) {
next_direc[k]=active_direc[k];
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]=='-'||grid[active_coordinates[k][1]][active_coordinates[k][0]]=='|'){
next_direc[k]=active_direc[k];
}
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]=='\\'){
if(active_direc[k]== 0) next_direc[k] = 3;
else if(active_direc[k]==1) next_direc[k] = 2;
else if(active_direc[k]==2) next_direc[k] = 1;
else if(active_direc[k]==3) next_direc[k] = 0;
}
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]=='/'){
if(active_direc[k]== 0) {
next_direc[k] = 1;
}
else if(active_direc[k]==1) {
next_direc[k] = 0;
}
else if(active_direc[k]==2) {
next_direc[k] = 3;
}
else if(active_direc[k]==3) {
next_direc[k] = 2;
}
}
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]=='+'){
next_direc[k]=getSmartDirectionAtCrossing(k);
}
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]>=65 && grid[active_coordinates[k][1]][active_coordinates[k][0]]<=90){
int I=grid[active_coordinates[k][1]][active_coordinates[k][0]]-'A';
// The word of the current state (State0 or State1 in the level file) decides the route
string label;
if(numbers[I][0]==0) label=straight[I];
else label=turn[I];
if(label=="TURN") {
next_direc[k]=bestOpenDirection(k,false);
}
else if(label=="LEFT") {
next_direc[k]=sideOrStraight(k,(active_direc[k]+3)%4);
}
else if(label=="RIGHT") {
next_direc[k]=sideOrStraight(k,(active_direc[k]+1)%4);
}
else {
next_direc[k]=active_direc[k];
}
}
return 1;
}
// ----------------------------------------------------------------------------
// SMART ROUTING AT CROSSING - Route train to its matched destination
// ----------------------------------------------------------------------------
// Choose best direction at '+' toward destination.
// ----------------------------------------------------------------------------
int getSmartDirectionAtCrossing(int trainIdx) {
return bestOpenDirection(trainIdx,true);
}

// ----------------------------------------------------------------------------
// DETERMINE ALL ROUTES (PHASE 2)
// ----------------------------------------------------------------------------
// Fill next positions/directions for all trains.
// ----------------------------------------------------------------------------
void determineAllRoutes() {
//it tooks the active train coordinate , and see on that train's coordinate if it is - or | it do nothing if / or \ it turns 90 degree the train checks switch state and wrt state lets the train go
for(int k=0;k<t_train;k++){
        if(trainActive[k]){
            held[k]=false;
            blocked[k]=false;
            getNextDirection(k);
            determineNextPosition(k);
        }}
}

// ----------------------------------------------------------------------------
// MOVE ALL TRAINS (PHASE 5)
// ----------------------------------------------------------------------------
// Move trains. A train that is held waits on its tile. A train that tries to
// enter a tile that is not track crashes.
// ----------------------------------------------------------------------------
void moveAllTrains() {
for(int i=0; i<t_train; i++) {
if(!trainActive[i])
continue;
energyUsed++;
if(held[i]) {
waitTotal++;
prev_coordinates[i][0]=active_coordinates[i][0];
prev_coordinates[i][1]=active_coordinates[i][1];
continue;
}
if(!isInBounds(next_coor[i][1],next_coor[i][0]) || !isTrackTile(next_coor[i][1],next_coor[i][0])) {
endTrain(i,2);
continue;
}
if(isSwitchTile(next_coor[i][1],next_coor[i][0])) {
int I=grid[next_coor[i][1]][next_coor[i][0]]-'A';
if(switch_exists[I] && signalShown[I]==2 && !(next_coor[i][0]==fdest[i][0] && next_coor[i][1]==fdest[i][1])) {
signalViolations++;
}
}
prev_coordinates[i][0]=active_coordinates[i][0];
prev_coordinates[i][1]=active_coordinates[i][1];
active_coordinates[i][0]=next_coor[i][0];
active_coordinates[i][1]=next_coor[i][1];
active_direc[i]=next_direc[i];
// Safety buffer: one tick delay before the train can move again
if(grid[active_coordinates[i][1]][active_coordinates[i][0]]=='=') {
delay_t[i]++;
}
// Rain: one extra wait tick after RAIN_N moves
if(weather=="RAIN") {
rain_moves[i]++;
if(rain_moves[i]>=RAIN_N) {
delay_t[i]++;
rain_moves[i]=0;
}
}
}
}

// ----------------------------------------------------------------------------
// HELPERS FOR COLLISIONS
// ----------------------------------------------------------------------------
// Make a train wait on its tile this tick.
static void holdTrain(int k, bool isBlocked) {
next_coor[k][0]=active_coordinates[k][0];
next_coor[k][1]=active_coordinates[k][1];
held[k]=true;
if(isBlocked)
blocked[k]=true;
}

// True if the train does not move this tick.
static bool isStationary(int k) {
return(next_coor[k][0]==active_coordinates[k][0] && next_coor[k][1]==active_coordinates[k][1]);
}

// True if the planned route of train k (before any wait) leads to tile x,y.
static bool wantsTile(int k, int x, int y) {
int dx[4]={0,1,0,-1};
int dy[4]={-1,0,1,0};
int d=next_direc[k];
return(active_coordinates[k][0]+dx[d]==x && active_coordinates[k][1]+dy[d]==y);
}

// True if the train is in the 3x3 area of a switch group that has a halt running.
static bool isHalted(int k) {
for(int dy=-1; dy<=1; dy++) {
for(int dx=-1; dx<=1; dx++) {
int x=active_coordinates[k][0]+dx;
int y=active_coordinates[k][1]+dy;
if(isInBounds(y,x) && isSwitchTile(y,x)) {
int I=grid[y][x]-'A';
if(switch_exists[I] && halt_timer[I]>0)
return true;
}
}
}
return false;
}

// A collision happened at this train: halt every switch group next to it.
static void haltNear(int k) {
for(int dy=-1; dy<=1; dy++) {
for(int dx=-1; dx<=1; dx++) {
int x=active_coordinates[k][0]+dx;
int y=active_coordinates[k][1]+dy;
if(isInBounds(y,x) && isSwitchTile(y,x)) {
applyEmergencyHalt(grid[y][x]-'A');
}
}
}
}

// ----------------------------------------------------------------------------
// DETECT COLLISIONS WITH PRIORITY SYSTEM
// ----------------------------------------------------------------------------
// Rules from the manual (same tile, crossing and head-on swap):
//  - the train with the higher distance to its destination moves, the others wait
//  - if the highest distance is shared, those trains crash
// A train also waits if the tile it wants is taken by a train that is not moving.
// ----------------------------------------------------------------------------
void detectCollisions() {
for(int i=0; i<t_train; i++) {
if(!trainActive[i])
continue;
cdist[i]=abs(active_coordinates[i][0] - fdest[i][0]) + abs(active_coordinates[i][1] - fdest[i][1]);
// delay from a safety buffer or rain
if(delay_t[i]>0) {
delay_t[i]--;
holdTrain(i,false);
}
}
bool changed=true;
int passes=0;
while(changed && passes<100) {
changed=false;
passes++;
// trains inside a halted area wait
for(int i=0; i<t_train; i++) {
if(trainActive[i] && !isStationary(i) && isHalted(i)) {
holdTrain(i,false);
changed=true;
}
}
// several trains want the same tile (also covers the crossing +)
for(int i=0; i<t_train; i++) {
if(!trainActive[i] || isStationary(i))
continue;
int group[30];
int g=0;
group[g++]=i;
for(int j=i+1; j<t_train; j++) {
if(trainActive[j] && !isStationary(j) && next_coor[j][0]==next_coor[i][0] && next_coor[j][1]==next_coor[i][1])
group[g++]=j;
}
if(g<2)
continue;
changed=true;
conflict=1;
int best=0;
for(int a=0; a<g; a++) {
if(cdist[group[a]]>best)
best=cdist[group[a]];
}
int tied=0;
for(int a=0; a<g; a++) {
if(cdist[group[a]]==best)
tied++;
}
if(tied>=2) {
cout<<"Collision at ("<<next_coor[i][0]<<","<<next_coor[i][1]<<"): trains";
for(int a=0; a<g; a++) {
if(cdist[group[a]]==best)
cout<<" "<<group[a];
}
cout<<" crash (same distance)\n";
for(int a=0; a<g; a++) {
if(cdist[group[a]]==best)
haltNear(group[a]);
}
}
for(int a=0; a<g; a++) {
int t=group[a];
if(cdist[t]==best && tied>=2)
endTrain(t,2);
else if(cdist[t]<best || tied>=2)
holdTrain(t,true);
}
}
// two trains swap tiles
for(int i=0; i<t_train; i++) {
for(int j=i+1; j<t_train; j++) {
if(!trainActive[i] || !trainActive[j] || isStationary(i) || isStationary(j))
continue;
if(next_coor[i][0]==active_coordinates[j][0] && next_coor[i][1]==active_coordinates[j][1] && next_coor[j][0]==active_coordinates[i][0] && next_coor[j][1]==active_coordinates[i][1]) {
changed=true;
conflict=1;
if(cdist[i]>cdist[j]) {
holdTrain(j,true);
}
else if(cdist[j]>cdist[i]) {
holdTrain(i,true);
}
else {
cout<<"Collision (head-on swap): trains "<<i<<" and "<<j<<" crash (same distance)\n";
haltNear(i);
haltNear(j);
endTrain(i,2);
endTrain(j,2);
}
}
}
}
// a train waits if the tile it wants is held by a train that does not move
// (exception: the winner of a head-on swap, because the manual lets it move)
for(int i=0; i<t_train; i++) {
if(!trainActive[i] || isStationary(i))
continue;
for(int j=0; j<t_train; j++) {
if(j!=i && trainActive[j] && isStationary(j) && active_coordinates[j][0]==next_coor[i][0] && active_coordinates[j][1]==next_coor[i][1]) {
if(wantsTile(j,active_coordinates[i][0],active_coordinates[i][1]))
continue;
holdTrain(i,true);
changed=true;
break;
}
}
}
}
}
// ----------------------------------------------------------------------------
// CHECK ARRIVALS
// ----------------------------------------------------------------------------
// Mark trains that reached destinations.
// ----------------------------------------------------------------------------
void checkArrivals() {
for(int i=0; i<t_train; i++) {
if(!trainActive[i])
continue;
int x=active_coordinates[i][0];
int y=active_coordinates[i][1];
if(grid[y][x]=='D') {
if(x==fdest[i][0] && y==fdest[i][1]) {
endTrain(i,1);
}
}
}
}
// ----------------------------------------------------------------------------
// APPLY EMERGENCY HALT
// ----------------------------------------------------------------------------
// Start a halt on switch group I. Trains in the 3x3 area around its tiles wait
// for HALT_TICKS ticks. Counters and switch states keep running. A group does not
// start a new halt on the tick right after its last one ended.
// ----------------------------------------------------------------------------
void applyEmergencyHalt(int I) {
if(I<0 || I>=26 || !switch_exists[I])
return;
if(halt_timer[I]==0 && tick!=halt_end[I]+1) {
halt_timer[I]=HALT_TICKS;
cout<<"Emergency halt on switch "<<(char)('A'+I)<<" for "<<HALT_TICKS<<" ticks\n";
}
}

// ----------------------------------------------------------------------------
// UPDATE EMERGENCY HALT
// ----------------------------------------------------------------------------
// Decrement timers and disable when done.
// ----------------------------------------------------------------------------
void updateEmergencyHalt() {
for(int i=0; i<26; i++) {
if(halt_timer[i]>0) {
halt_timer[i]--;
if(halt_timer[i]==0) {
halt_end[i]=tick;
}
}
}
}
