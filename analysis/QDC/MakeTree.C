#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TF1.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using std::cout;
using std::endl;

void MakeTree(TString const& input, TString const& output, TString const& output_root = "TreeQDC.root"){
	
	FILE *data;
	
        data = fopen(input,"rt");
	if(data == NULL){
	   printf("Arquivo input não encontrado");
	   exit(0);
	}
	
	char urlNull[]="Null.txt";

	FILE *arqNull;
	arqNull = fopen(urlNull,"w");
	if(arqNull == NULL){
	   printf("Não foi possivel abrir o arquivo NULL\n");
	   exit(0);
	}	

	FILE *arq;
        arq = fopen(output,"w"); 
	if(arq == NULL){
	   printf("Não foi possivel abrir o arquivo output\n");
	   exit(0);
	}
	
	TFile *file = new TFile(output_root,"recreate");
   	TTree *t1 = new TTree("t1","Tree com dados QDC");
	
        const int NSEARCHMAX = 100;
        //double resolution = 0.100; // 0.1 pC/count
        double resolution = 0.025; // 0.025 pC/count

	int event, prev_event, null_events, channel0_0, counts0_0, channel0_1, counts0_1, overflow, underflow;
        double charge;

        null_events = 0; 
        event = -1;
        prev_event = -1;

	t1->Branch("event",&event,"event/I");
   	t1->Branch("counts",&counts0_1,"counts/I");
   	t1->Branch("charge",&charge,"charge/D");
	
        fpos_t pos;
        bool valid = true;

	while( !feof(data) ){

	   int ret = -1;
	   char line[50];

	   channel0_0 = -1;
	   counts0_0 = -1;
	   channel0_1 = -1;
	   counts0_1 = -1;
	   charge = -999.;

	   if ( !valid ) {
	      cout << "Find correct start line." << endl;
	      for(int i=0; i < NSEARCHMAX; ++i) {
		 cout << "  Iteration " << (i+1) << endl;
		 cout << "  Prev. event " << prev_event << endl;

		 //ret = fscanf(data,"Ev %d", &event);
		 char* str;
		 fgetpos( data, &pos ); 
		 str = fgets(line,sizeof(line),data);
		 if( !strncmp(line,"EV",2) ){ 
		    ret = sscanf(line,"EV %d \n", &event);
		    cout << "Event " << event << endl;
		    if ( ret && ( event == (prev_event + 1) ) ) { valid = true; cout << "Found correct position." << endl; break; }
		 } 
	      }
	      if ( !valid ) break;
	   } else{
	      prev_event = event;
	      cout << "Prev. event " << prev_event << endl;
	      //fgetpos( data, &pos ); 
	      //ret = fscanf(data,"Ev %d", &event);
	      char* str;
	      fgetpos( data, &pos ); 
	      str = fgets(line,sizeof(line),data);
              //puts( line );
	      if( !strncmp(line,"EV",2) ){ 
		 ret = sscanf(line,"EV %d \n", &event);
		 if( !ret ) { valid = false; fsetpos( data, &pos ); continue; }
	      }
	      cout << "Event " << event << endl;

	      if( event == 0 && prev_event == -1 ) prev_event = 0;
	      if( event == 1 && prev_event == -1 ) prev_event = 0;
	      else if ( event != (prev_event + 1) ) { valid = false; fsetpos( data, &pos ); continue; }

	      if( feof(data) ) break;
	   }

           char* str_data;
	   fgetpos( data, &pos );
	   str_data = fgets(line,sizeof(line),data);
	   if( !strncmp(line,"CH",2) ){ 
	      ret = sscanf(line,"CH %d ADC %d UN %d OV %d \n", &channel0_0,&counts0_0,&underflow,&overflow);
	      if( !ret ) { valid = false; fsetpos( data, &pos ); prev_event = event; continue; }
	   }
	   cout << "Channel: " << channel0_0 << " Counts: " << counts0_0 << endl;
	   if ( channel0_0 != 0 ) { valid = false; fsetpos( data, &pos ); prev_event = event; continue; }

	   fgetpos( data, &pos );
	   str_data = fgets(line,sizeof(line),data);
	   if( !strncmp(line,"CH",2) ){ 
	      ret = sscanf(line,"CH %d ADC %d UN %d OV %d \n", &channel0_1,&counts0_1,&underflow,&overflow);
	      if( !ret ) { valid = false; fsetpos( data, &pos ); prev_event = event; continue; }
	   }
	   cout << "Channel: " << channel0_1 << " Counts: " << counts0_1 << endl;
	   if ( channel0_1 != 0 ) { valid = false; fsetpos( data, &pos ); prev_event = event; continue; }

	   if( counts0_0 < 0 || counts0_1 < 0 ){
	      ++null_events;
	      fprintf(arqNull, "Eventos Nulo: %d Event: %d Counts: %d\n",null_events,event,counts0_1);
	   }
	   else{
	      charge = resolution*counts0_1;

	      fprintf(arq,"Evento: %d Counts: %d Charge: %f \n", event, counts0_1, charge);
	      t1->Fill();
	   }

	}
	t1->Write();
   	t1->Print();
   	file->Close();

        fclose(data);
	fclose(arq);
	fclose(arqNull);
	return ;

}
