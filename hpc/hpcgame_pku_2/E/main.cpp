#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>
#include <string>

using namespace std;

const int TREE  = 1;
const int FIRE  = 2;
const int ASH   = 3;
const int EMPTY = 0;

struct Event {
    int ts;      // 事件触发的时间步
    int type;    // 事件类型：1（天降惊雷），2（妙手回春）
    int x1, y1;  // 事件的坐标或区域范围
    int x2, y2;  // 仅用于“妙手回春”事件
};

int **forest;
int **prv;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0, size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }
#ifdef debug
    string debug_file = "debug_" + to_string(rank) + ".txt";
#endif
    const char* input_file  = argv[1];
    const char* output_file = argv[2];

    int n = 0, m = 0, T = 0;
    int len = 0;
    Event events[2000];

    if (rank == 0) {
        //read mata info

        freopen(input_file, "r", stdin);
        std::cin >> n >> m >> T;
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&T, 1, MPI_INT, 0, MPI_COMM_WORLD);
        len = n / size;
        forest = new int*[len + 2 + n % size];
        prv    = new int*[len + 2 + n % size];

        for (int i = 0; i < len + 2 + n % size; i++) {
            forest[i] = new int[n + 2];
            prv[i] = new int[n + 2];
        }
        for(int i = 0; i <= len + 1; i++) {
            for(int j = 0; j <= n + 1; j++) {
                forest[i][j] = EMPTY;
                prv[i][j] = EMPTY;
            }
        }//init

        for (int i = 1; i <= n - len * size + len; i++) {
            for (int j = 1; j <= n; j++) {
                std::cin >> forest[i][j];
            }
        }//read self
        

        #ifdef debug
        freopen(debug_file.c_str(), "w", stdout);
        printf("n=%d m=%d T=%d\n", n, m, T);
        for(int i = 0;i <= len + 1; i++) {
            for(int j = 1;j <= n; j++) {
                printf("%d ", forest[i][j]);
            }
            printf("\n");
        }
        puts("");
        #endif


        int *tmp = new int[(n + 2) * (len + 2)];//send buffer
        for(int i = 0; i <= len + 1; i++) {
            for(int j = 1;j <= n; j++) {
                tmp[i * n + j] = 0;
            }
        }

        // 主进程发送数据给其他进程的修改
for (int thread_cnt = 1; thread_cnt < size; thread_cnt++) {
    int other_len = n / size; // 其他进程的len
    int start_row = (n - (n % size)) + 1 + (thread_cnt - 1) * other_len; // 根据实际调整起始行
    // 填充数据到tmp，仅发送other_len行
    for (int i = 1; i <= other_len; i++) {
        for (int j = 1; j <= n; j++) {
            std::cin >> tmp[i * (n + 2) + j];
        }
    }
    MPI_Send(tmp, (other_len + 2) * (n + 2), MPI_INT, thread_cnt, 0, MPI_COMM_WORLD);
}
        for (int i = 0; i < m; i++) {
            std::cin >> events[i].ts >> events[i].type >> events[i].x1 >> events[i].y1;
            if (events[i].type == 2) {
                std::cin >> events[i].x2 >> events[i].y2;
            }
        }
        delete[] tmp;
    }
    else {
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&T, 1, MPI_INT, 0, MPI_COMM_WORLD);
        len = n / size;
        forest = new int*[len + 2];
        prv    = new int*[len + 2];
        for (int i = 0; i < len + 2; i++) {
            forest[i] = new int[n + 2];
            prv[i] = new int[n + 2];
        }
        int *tmp = new int[(len + 2) * (n + 2)];
        MPI_Recv(tmp, (len + 2) * (n + 2), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i <= len + 1; i++) {
            for (int j = 0; j <= n + 1; j++) {
                forest[i][j] = tmp[i * (n + 2) + j];
            }
        }
        #ifdef debug
        freopen(debug_file.c_str(), "w", stdout);
        printf("n=%d m=%d T=%d\n", n, m, T);
        for(int i = 0;i <= len + 1; i++) {
            for(int j = 1;j <= n; j++) {
                printf("%d ", forest[i][j]);
            }
            printf("\n");
        }
        puts("");
        #endif
    }

    // 定义 MPI 数据类型用于 Event 结构
    MPI_Datatype MPI_EventType;
    int blocklengths[6] = { 1, 1, 1, 1, 1,1 };
    MPI_Datatype types[6] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT };
    MPI_Aint offsets[6];
    offsets[0] = offsetof(Event, ts);
    offsets[1] = offsetof(Event, type);
    offsets[2] = offsetof(Event, x1);
    offsets[3] = offsetof(Event, y1);
    offsets[4] = offsetof(Event, x2);
    offsets[5] = offsetof(Event, y2);
    MPI_Type_create_struct(6, blocklengths, offsets, types, &MPI_EventType);
    MPI_Type_commit(&MPI_EventType);

    MPI_Bcast(events, m, MPI_EventType, 0, MPI_COMM_WORLD);

    // 计算每个进程负责的区域
    len = (rank == 0) ? (n / size + n % size) : (n / size);
    int i_lb = rank * len + ((rank == 0) ? 0 : (n - len * size)) + 1;
    int i_ub = i_lb + len - 1;
    #ifdef debug
    freopen(debug_file.c_str(), "a", stdout);
    printf("rank = %d i_lb = %d i_ub = %d\n", rank, i_lb, i_ub);
    #endif
    int last_event = 0;
    MPI_Barrier(MPI_COMM_WORLD);

    for (int t = 1; t <= T; t++) {
        for (int i = 0; i <= len + 1; i++) {
            for (int j = 1; j <= n; j++) {
                prv[i][j] = forest[i][j];
            }
        }

        // 处理事件
        if (last_event < m && events[last_event].ts == t) {
            #ifdef debug
            printf("rank = %d live! t = %d last_event=%d\n", rank, t, last_event);
            printf("last event: ts=%d type=%d x1=%d y1=%d x2=%d y2=%d\n", events[last_event].ts, events[last_event].type, events[last_event].x1, events[last_event].y1, events[last_event].x2, events[last_event].y2);
            #endif
            if (events[last_event].type == 1) {
                int x_global = events[last_event].x1 + 1;
                int y_global = events[last_event].y1 + 1;
                #ifdef debug
                printf("thunder [%d,%d]\n", x_global, y_global);
                #endif
                if (x_global >= i_lb && x_global <= i_ub) {
                    int x_local = x_global - i_lb + 1;
                    if (prv[x_local][y_global] == TREE) {
                        prv[x_local][y_global] = FIRE;
                    }
                }
            }
            else {
                #ifdef debug
                printf("rank = %d live! start event bound:[%d,%d]*[%d,%d]\n", rank, std::max(events[last_event].x1 + 1, i_lb), std::min(i_ub, events[last_event].x2 + 1), events[last_event].y1 + 1, events[last_event].y2 + 1);
                #endif
                for (int i = std::max(events[last_event].x1 + 1, i_lb); i <= std::min(i_ub, events[last_event].x2 + 1); i++) {
                    for (int j = events[last_event].y1 + 1; j <= events[last_event].y2 + 1; j++) {
                        if (prv[i - i_lb + 1][j] == ASH) {
                            #ifdef debug
                            printf("cure [%d,%d]\n",  i, j);
                            #endif
                            prv[i - i_lb + 1][j] = TREE;
                        }
                    }
                }
            }
            last_event++;
#ifdef debug
            printf("rank = %d live! event\n", rank);
            freopen(debug_file.c_str(), "a", stdout);
            for(int i = 0;i <= len + 1; i++) {
                for(int j = 1;j <= n; j++) {
                    printf("%d ", prv[i][j]);
                }
                printf("\n");
            }
            puts("");
#endif
        }


        MPI_Request up_send, down_send;
        if (rank != 0) {
            MPI_Isend(&prv[1][0], n + 2, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &up_send);
        }
        if (rank != size - 1) {
            MPI_Isend(&prv[len][0], n + 2, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &down_send);
        }
        MPI_Request up_recv, down_recv;
        if (rank != 0)
            MPI_Irecv(&prv[0][0], n + 2, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &up_recv);
        if (rank != size - 1)
            MPI_Irecv(&prv[len + 1][0], n + 2, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &down_recv);

        // 状态转移
        for (int i = 2; i <= len - 1; i++) {
            for (int j = 1; j <= n; j++) {
                if (prv[i][j] == TREE) {
                    bool is_adjacent_fire = false;
                    if (prv[i - 1][j] == FIRE)
                        is_adjacent_fire = true;
                    if (prv[i + 1][j] == FIRE)
                        is_adjacent_fire = true;
                    if (prv[i][j - 1] == FIRE)
                        is_adjacent_fire = true;
                    if (prv[i][j + 1] == FIRE)
                        is_adjacent_fire = true;
                    if (is_adjacent_fire) {
                        forest[i][j] = FIRE;
                    }
                    else {
                        forest[i][j] = TREE;
                    }
                }
                else if (prv[i][j] == FIRE) {
                    forest[i][j] = ASH;
                }
                else {
                    forest[i][j] = prv[i][j];
                }
            }
        }

        

        // 等待发送完成
        if (rank != 0) {
            MPI_Wait(&up_send, MPI_STATUS_IGNORE);
        }
        if (rank != size - 1) {
            MPI_Wait(&down_send, MPI_STATUS_IGNORE);
        }
        if (rank != 0) {
            MPI_Wait(&up_recv, MPI_STATUS_IGNORE);
        }
        if (rank != size - 1) {
            MPI_Wait(&down_recv, MPI_STATUS_IGNORE);
        }

        for (int j = 1; j <= n; j++) {
            int i = 1;
            if (prv[i][j] == TREE) {
                bool is_adjacent_fire = false;
                if (prv[i - 1][j] == FIRE)
                    is_adjacent_fire = true;
                if (prv[i + 1][j] == FIRE)
                    is_adjacent_fire = true;
                if (prv[i][j - 1] == FIRE)
                    is_adjacent_fire = true;
                if (prv[i][j + 1] == FIRE)
                    is_adjacent_fire = true;
                if (is_adjacent_fire) {
                    forest[i][j] = FIRE;
                }
                else {
                    forest[i][j] = TREE;
                }
            }
            else if (prv[i][j] == FIRE) {
                forest[i][j] = ASH;
            }
            else {
                forest[i][j] = prv[i][j];
            }
        }
        for (int j = 1; j <= n; j++) {
            int i = len;
            if (prv[i][j] == TREE) {
                bool is_adjacent_fire = false;
                if (prv[i - 1][j] == FIRE)
                    is_adjacent_fire = true;
                if (prv[i + 1][j] == FIRE)
                    is_adjacent_fire = true;
                if (prv[i][j - 1] == FIRE)
                    is_adjacent_fire = true;
                if (prv[i][j + 1] == FIRE)
                    is_adjacent_fire = true;
                if (is_adjacent_fire) {
                    forest[i][j] = FIRE;
                }
                else {
                    forest[i][j] = TREE;
                }
            }
            else if (prv[i][j] == FIRE) {
                forest[i][j] = ASH;
            }
            else {
                forest[i][j] = prv[i][j];
            }
        }
#ifdef debug
        printf("%d transfered!\n",t);
        freopen(debug_file.c_str(), "a", stdout);
        for(int i = 0;i <= len + 1; i++) {
            for(int j = 1;j <= n; j++) {
                printf("%d ", forest[i][j]);
            }
            printf("\n");
        }
        puts("");
#endif
    }

    if (rank == 0) {
    freopen(output_file, "w", stdout);
    for (int i = 1; i <= len; i++) {
        for (int j = 1; j <= n; j++) {
            std::cout << forest[i][j] << " ";
        }
        std::cout << std::endl;
    }
    len = n / size;
    int *rev = new int[(len + 2) * (n + 2)];
    for (int k = 1; k < size; k++) {
        MPI_Recv(rev, (len + 2) * (n + 2), MPI_INT, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 1; i <= len; i++) {
            for (int j = 1; j <= n; j++) {
                std::cout << rev[i * (n + 2) + j] << " "; // Corrected indexing
            }
            std::cout << std::endl;
        }
    }
    delete[] rev;
} else {
    int *tmp = new int[(len + 2) * (n + 2)];
    for (int i = 0; i <= len + 1; i++) {
        for (int j = 0; j <= n + 1; j++) {
            tmp[i * (n + 2) + j] = forest[i][j]; // Corrected indexing
        }
    }
    MPI_Send(tmp, (len + 2) * (n + 2), MPI_INT, 0, 0, MPI_COMM_WORLD);
    delete[] tmp;
}
// 释放forest和prv
for (int i = 0; i < len + 2; ++i) {
    delete[] forest[i];
    delete[] prv[i];
}
delete[] forest;
delete[] prv;
    MPI_Finalize();
    return 0;
}