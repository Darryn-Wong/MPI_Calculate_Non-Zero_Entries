#include <stdio.h>
#include <fstream>
#include <climits>
#include <iostream>
#include <vector>
#include "mpi.h"
using namespace std;
// helper funtion to print custom vector
void print_custom_vector(vector<int> &v) {
  for(int i =0; i < v.size();i+=3) {
    cout << "[" << v[i]+1 << "," << v[i+1]+1 << "]" << v[i+2] <<endl;
  }
}
int main(int argc, char *argv[])   {
  string input_file = "input.txt";
  if(argc == 2) {
    input_file = argv[1];
  }
  int count = 0;
  int sendcount = 0;
  int myid,size,size_p,spread_size,cur_index;
  int size_c,size_r;
  int **input,*subinput;
  // MPI_Comm *workers = new MPI_Comm;
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  MPI_Comm_size(MPI_COMM_WORLD,&size_p);
  ifstream inputStream;
  inputStream.open(input_file);
  inputStream >> size_c >> size_r;
  inputStream.close();
  spread_size = size/size_p;
  if(myid==0) {
    // Rank 0 parses the input
    cout << "number of nodes " << size_p << "\n";
    cout << "======== Rank 0 Node parses the input file =========\n";
    ifstream inputStream;
    inputStream.open(input_file);
    inputStream >> size_c >> size_r;
    input = (int **)malloc(size_c * sizeof(int *));
    for (int i=0; i < size_c; i++) { input[i] = (int *)malloc(size_r * sizeof(int));}
    for(int i = 0; i < size_r; i++)
        for(int j = 0; j < size_r; j++) {
          inputStream >> input[i][j];
        }
    inputStream.close();
    cout << size_c << " " << size_r << endl;
    for(int i = 0; i < size_c; i++){
        for(int j = 0; j < size_r; j++){
            cout << input[i][j] << " ";
          }
        cout << "\n";
    }
    cur_index = 0;
    cout << "======== Rank 0 Start Assignning Work =========\n";
    while(cur_index < size_c) {
      MPI_Send(input[cur_index], size_r, MPI_INT, cur_index % (size_p), cur_index, MPI_COMM_WORLD);
      printf("rank %d SEND rows %d to rank %d\n",myid,cur_index + 1,cur_index % size_p);
      cur_index++;
    }
    cout << "======== Start Computating Segment =========\n";
  }
  // each process computes their segment
    cur_index = myid;
    int tag = 0;
    while(cur_index < size_c) {
      int small_array[size_r];
      MPI_Status status;
      MPI_Recv(small_array, size_r, MPI_INT, 0, cur_index, MPI_COMM_WORLD, &status);
      int *send_data = (int *)malloc(size_r * sizeof(int)*3 + 1);
      int small_array_count = 1;
      for(int i = 0; i < size_r; i++) {
        if(small_array[i] != 0){
          send_data[small_array_count] = cur_index;
          send_data[small_array_count+1] = i;
          send_data[small_array_count+2] = small_array[i];
          // cout << cur_index << ":" << i << ":"<<small_array[i] << ":" <<  send_data[small_array_count+2]<< endl;
          small_array_count += 3;
          }
      }
      send_data[0] = small_array_count;
      MPI_Send(send_data, size_r*3+1, MPI_INT, 0, INT_MAX, MPI_COMM_WORLD);
      cur_index += size_p;
    }
    if(myid == 0) {
      // Rank 0 Reciving the results and printing it out
      cout << "===== RANK 0 Reciveved All the Results =====\n";
      std::vector<int> v;
      for(int i = 0; i < size_c;i++) {
        MPI_Status status1;
        int computed_data_segment[size_r * 3 + 1];
        MPI_Recv(computed_data_segment, size_r * 3 + 1, MPI_INT, MPI_ANY_SOURCE, INT_MAX, MPI_COMM_WORLD, &status1);
        // std::cout << "Received from process " << status1.MPI_SOURCE
        //           << "; with tag " << status1.MPI_TAG << "; error " << status1.MPI_ERROR << "; count " << status1._ucount << "; canceled " << status1._cancelled << std::endl;
        for(int i = 1; i < computed_data_segment[0];i++) {
          v.push_back(computed_data_segment[i]);
        }
      }
      print_custom_vector(v);
    }
  MPI_Finalize();
}
