
#define SEG 8.5
#define CR_TO_SELF_0 SEG
#define CR_TO_SELF_1 (2*SEG)
#define CR_TO_SELF_2 (3*SEG)
#define CR_TO_SELF_3 (4*SEG)
#define CR_TO_SELF_4 (5*SEG)
#define CR_TO_SELF_5 (6*SEG)
#define CR_TO_CNTR_0 CR_TO_SELF_5
#define CR_TO_CNTR_1 CR_TO_SELF_4
#define CR_TO_CNTR_2 CR_TO_SELF_3
#define CR_TO_CNTR_3 CR_TO_SELF_2
#define CR_TO_CNTR_4 CR_TO_SELF_1
#define CR_TO_CNTR_5 CR_TO_SELF_0
#define CR_TO_ADJ_CR_0 (CR_TO_SELF_0 + (6*12))
#define CR_TO_ADJ_CR_1 (CR_TO_SELF_1 + (5*12))
#define CR_TO_ADJ_CR_2 (CR_TO_SELF_2 + (4*12))
#define CR_TO_ADJ_CR_3 (CR_TO_SELF_3 + (3*12))
#define CR_TO_ADJ_CR_4 (CR_TO_SELF_4 + (2*12))
#define CR_TO_ADJ_CR_5 (CR_TO_SELF_5 + (1*12))
#define CR_TO_OPP_CR_0 (CR_TO_SELF_0 + (2*CR_TO_CNTR_0))
#define CR_TO_OPP_CR_1 (CR_TO_SELF_1 + (2*CR_TO_CNTR_1))
#define CR_TO_OPP_CR_2 (CR_TO_SELF_2 + (2*CR_TO_CNTR_2))
#define CR_TO_OPP_CR_3 (CR_TO_SELF_3 + (2*CR_TO_CNTR_3))
#define CR_TO_OPP_CR_4 (CR_TO_SELF_4 + (2*CR_TO_CNTR_4))
#define CR_TO_OPP_CR_5 (CR_TO_SELF_5 + (2*CR_TO_CNTR_5))

#define EDGE_TO_SELF_0 6
#define EDGE_TO_SELF_1 (2*6)
#define EDGE_TO_SELF_2 (3*6)
#define EDGE_TO_SELF_3 (4*6)
#define EDGE_TO_SELF_4 (5*6)
#define EDGE_TO_SELF_5 (6*6)
#define EDGE_TO_CNTR_0 EDGE_TO_SELF_5
#define EDGE_TO_CNTR_1 EDGE_TO_SELF_4
#define EDGE_TO_CNTR_2 EDGE_TO_SELF_3
#define EDGE_TO_CNTR_3 EDGE_TO_SELF_2
#define EDGE_TO_CNTR_4 EDGE_TO_SELF_1
#define EDGE_TO_CNTR_5 EDGE_TO_SELF_0
#define EDGE_TO_EDGE_0 (EDGE_TO_SELF_0 + (2*EDGE_TO_CNTR_0))
#define EDGE_TO_EDGE_1 (EDGE_TO_SELF_1 + (2*EDGE_TO_CNTR_1))
#define EDGE_TO_EDGE_2 (EDGE_TO_SELF_2 + (2*EDGE_TO_CNTR_2))
#define EDGE_TO_EDGE_3 (EDGE_TO_SELF_3 + (2*EDGE_TO_CNTR_3))
#define EDGE_TO_EDGE_4 (EDGE_TO_SELF_4 + (2*EDGE_TO_CNTR_4))
#define EDGE_TO_EDGE_5 (EDGE_TO_SELF_5 + (2*EDGE_TO_CNTR_5))

#define CR_TO_ADJ_EDGE_0 (EDGE_TO_SELF_0 + (6*6))
#define CR_TO_ADJ_EDGE_1 (EDGE_TO_SELF_1 + (5*6))
#define CR_TO_ADJ_EDGE_2 (EDGE_TO_SELF_2 + (4*6))
#define CR_TO_ADJ_EDGE_3 (EDGE_TO_SELF_3 + (3*6))
#define CR_TO_ADJ_EDGE_4 (EDGE_TO_SELF_4 + (2*6))
#define CR_TO_ADJ_EDGE_5 (EDGE_TO_SELF_5 + (1*6))
#define CR_TO_OPP_EDGE_0 (EDGE_TO_SELF_0 + EDGE_TO_CNTR_0 + CR_TO_CNTR_0)
#define CR_TO_OPP_EDGE_1 (EDGE_TO_SELF_1 + EDGE_TO_CNTR_1 + CR_TO_CNTR_1)
#define CR_TO_OPP_EDGE_2 (EDGE_TO_SELF_2 + EDGE_TO_CNTR_2 + CR_TO_CNTR_2)
#define CR_TO_OPP_EDGE_3 (EDGE_TO_SELF_3 + EDGE_TO_CNTR_3 + CR_TO_CNTR_3)
#define CR_TO_OPP_EDGE_4 (EDGE_TO_SELF_4 + EDGE_TO_CNTR_4 + CR_TO_CNTR_4)
#define CR_TO_OPP_EDGE_5 (EDGE_TO_SELF_5 + EDGE_TO_CNTR_5 + CR_TO_CNTR_5)


#define CR_SELF {CR_TO_SELF_0, CR_TO_SELF_1, CR_TO_SELF_2, CR_TO_SELF_3, CR_TO_SELF_4, CR_TO_SELF_5}
#define CR_CNTR {CR_TO_CNTR_0, CR_TO_CNTR_1, CR_TO_CNTR_2, CR_TO_CNTR_3, CR_TO_CNTR_4, CR_TO_CNTR_5}
#define CR_ADJ_CR {CR_TO_ADJ_CR_0, CR_TO_ADJ_CR_1, CR_TO_ADJ_CR_2, CR_TO_ADJ_CR_3, CR_TO_ADJ_CR_4, CR_TO_ADJ_CR_5}
#define CR_OPP_CR {CR_TO_OPP_CR_0, CR_TO_OPP_CR_1, CR_TO_OPP_CR_2, CR_TO_OPP_CR_3, CR_TO_OPP_CR_4, CR_TO_OPP_CR_5}
#define CR_ADJ_EDGE {CR_TO_ADJ_EDGE_0, CR_TO_ADJ_EDGE_1, CR_TO_ADJ_EDGE_2, CR_TO_ADJ_EDGE_3, CR_TO_ADJ_EDGE_4, CR_TO_ADJ_EDGE_5}
#define CR_OPP_EDGE {CR_TO_OPP_EDGE_0, CR_TO_OPP_EDGE_1, CR_TO_OPP_EDGE_2, CR_TO_OPP_EDGE_3, CR_TO_OPP_EDGE_4, CR_TO_OPP_EDGE_5}
#define EDGE_SELF {EDGE_TO_SELF_0, EDGE_TO_SELF_1, EDGE_TO_SELF_2, EDGE_TO_SELF_3, EDGE_TO_SELF_4, EDGE_TO_SELF_5}
#define EDGE_CNTR {EDGE_TO_CNTR_0, EDGE_TO_CNTR_1, EDGE_TO_CNTR_2, EDGE_TO_CNTR_3, EDGE_TO_CNTR_4, EDGE_TO_CNTR_5}
#define EDGE_EDGE {EDGE_TO_EDGE_0, EDGE_TO_EDGE_1, EDGE_TO_EDGE_2, EDGE_TO_EDGE_3, EDGE_TO_EDGE_4, EDGE_TO_EDGE_5}

#define GRAY_TO_DEST    {CR_CNTR, CR_SELF, CR_SELF, CR_SELF, CR_SELF, EDGE_SELF, EDGE_SELF}
#define YELLOW_TO_DEST  {CR_CNTR, CR_SELF, CR_ADJ_CR, CR_OPP_CR, CR_ADJ_CR, CR_ADJ_EDGE, CR_OPP_EDGE}
#define CYAN_TO_DEST    {CR_CNTR, CR_ADJ_CR, CR_SELF, CR_ADJ_CR, CR_OPP_CR, CR_ADJ_EDGE, CR_OPP_EDGE}
#define RED_TO_DEST     {CR_CNTR, CR_OPP_CR, CR_ADJ_CR, CR_SELF, CR_ADJ_CR, CR_OPP_EDGE, CR_ADJ_EDGE}
#define BLUE_TO_DEST    {CR_CNTR, CR_ADJ_CR, CR_OPP_CR, CR_ADJ_CR, CR_SELF, CR_OPP_EDGE, CR_ADJ_EDGE}
#define MAGENTA_TO_DEST {EDGE_CNTR, CR_ADJ_EDGE, CR_ADJ_EDGE, CR_OPP_EDGE, CR_OPP_EDGE, EDGE_SELF, EDGE_EDGE}
#define GREEN_TO_DEST   {EDGE_CNTR, CR_OPP_EDGE, CR_OPP_EDGE, CR_ADJ_EDGE, CR_ADJ_EDGE, EDGE_EDGE, EDGE_SELF}

#define LCURVE 2
#define LEFT 0
#define MIDDLE NUM_LINES/2
#define RIGHT NUM_LINES
#define RCURVE NUM_LINES-3
#define BACK NUM_LINES+2

#define FORK  3
#define CROSS 4
#define SPLIT 2

const char trackTable[NUM_COLORS][NUM_COLORS] = {
                        {BACK, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE},
                        {MIDDLE, BACK, RIGHT, MIDDLE, LEFT, RIGHT, LCURVE},
                        {MIDDLE, LEFT, BACK, RIGHT, MIDDLE, LEFT, RCURVE},
                        {MIDDLE, MIDDLE, LEFT, BACK, RIGHT, LCURVE, RIGHT},
                        {MIDDLE, RIGHT, MIDDLE, LEFT, BACK, RCURVE, LEFT},
                        {MIDDLE, LEFT, RIGHT, RCURVE, LCURVE, BACK, MIDDLE},
                        {MIDDLE, RCURVE, LCURVE, LEFT, RIGHT, MIDDLE, BACK}
                        }; 
const char nextCurrColor[NUM_COLORS][NUM_COLORS] = {
                        {GRAY, GRAY, GRAY, GRAY, GRAY, GRAY, GRAY},
                        {YELLOW, GRAY, GRAY, YELLOW, GRAY, GRAY, YELLOW},
                        {CYAN, GRAY, GRAY, GRAY, CYAN, GRAY, CYAN},
                        {RED, RED, GRAY, GRAY, GRAY, RED, GRAY},
                        {BLUE, GRAY, BLUE, GRAY, GRAY, BLUE, GRAY},
                        {MAGENTA, GRAY, GRAY, MAGENTA, MAGENTA, GRAY, MAGENTA},
                        {GREEN, GREEN, GREEN, GRAY, GRAY, GREEN, GRAY}
                        };
const char incSpot[NUM_COLORS][NUM_COLORS] = {
                        {0, -1, -1, -1, -1, -1, -1},
                        {1, -1, 0, 1, 0, 0, 1},
                        {1, 0, -1, 0, 1, 0, 1},
                        {1, 1, 0, -1, 0, 1, 0},
                        {1, 0, 1, 0, -1, 1, 0},
                        {1, 0, 0, 1, 1, -1, 1},
                        {1, 1, 1, 0, 0, 1, -1}
                        };  
const float distTable[NUM_COLORS][NUM_COLORS][6] = {
                GRAY_TO_DEST,
                YELLOW_TO_DEST,
                CYAN_TO_DEST,
                RED_TO_DEST,
                BLUE_TO_DEST,
                MAGENTA_TO_DEST,
                GREEN_TO_DEST
                };
#define travelDist distTable[currColor][destColor][spot]

void testDistTable(int currColor=0, int destColor=0, int spot=0){
    for (spot = 0; spot < 6; spot++){
        Serial.print("spot ");Serial.print(spot);Serial.print('\n');
        for (currColor = 0; currColor < NUM_COLORS; currColor++){
            showColor(currColor);Serial.print("_TO_");
            for (destColor = 0; destColor < NUM_COLORS; destColor++){
                showColor(destColor);Serial.print("=");
                Serial.print(distTable[currColor][destColor][spot]);
                Serial.print('\t');
            }
            Serial.print('\n');
        }
        Serial.print('\n');
    }
}
