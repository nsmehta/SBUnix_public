#include <sys/tarfs.h>
#include <sys/kprintf.h>

void assign_vfs_slot(int nindex, char nname[128], int nparent, uint64_t nsize, int ntype, uint64_t naddress) {
	strcpy(vfs[nindex].name, nname);
	vfs[nindex].parent = nparent;
	vfs[nindex].size = nsize;
	vfs[nindex].type = ntype;
	vfs[nindex].address = naddress;
	vfs[nindex].index = nindex;
}


int get_parent_index(char *file_name, int slice_count) {
  //kprintf("initial slice count = %d", slice_count);
  	int length = strlen(file_name);
	int i;
	int initial_slice_count = slice_count;
	for(i = length - 1; i >= 0 && slice_count; i--) {
	  //kprintf("file_name[i] = %c\n", file_name[i]);
		if(file_name[i] == '/') {
			slice_count--;
		}
	}
	//if the file_type was a file, not a folder
	if(initial_slice_count == 1 || initial_slice_count == 2) {
	  i++;
	}
	//kprintf("length = %d, i = %d", length, i);
	//kprintf("file_name = %s\n", file_name);
	if(i <= -1) {
	  //kprintf("returning from get_parent_index: %d\n", -1);
	  //kprintf("returning -1, file_name = %s, slice_count = %d\n", file_name, slice_count);
	  return -1;
	}
  
	char fname[128];
	//kprintf("filename=%s, i=%d\n", file_name, i);
	strsubcpy(fname, file_name, 0, i + 1);
	fname[i + 1] = '\0';
	//kprintf("fname=%s ", fname);

	for(i = 0; i < vfs_pointer; i++) {
	  //int strcmp_result = strcmp(fname, vfs[i].name);
	  //kprintf("strcmp result = %d\n", strcmp_result);
		if(!strcmp(fname, vfs[i].name)) {
		  //kprintf("matched filenames= %s %s\n", fname, vfs[i].name);
		  //kprintf("returning from get_parent_index: %d\n", i);
		  return i;
		}
	}
	//kprintf("returning from get_parent_index: %d\n", -2);
	return -2;
}

void tarfsInit() {

  assign_vfs_slot(0, "/", -1, 0, 5, 0);
  assign_vfs_slot(1, "/rootfs/", 0, 0, 5, 0);
	vfs_pointer = 2;
	char *root_directory_name = "/rootfs/";

	struct posix_header_ustar *p = (struct posix_header_ustar *)&_binary_tarfs_start;
	int file_number = 0;
	while(p <= (struct posix_header_ustar *)&_binary_tarfs_end && strlen(p->name) != 0) {
	  	//kprintf("pointer = %p, %p\n", p, (uint64_t)p + 10);
	  	char *root_directory = "";
		strcpy(root_directory, root_directory_name);
		char file_name[128];
		uint64_t file_size = oct2bin((unsigned char *)p->size, 11);
		//uint64_t file_tarfs_header = (uint64_t)p + file_number;
		char file_typeflag = p->typeflag[0];
		strcpy(file_name, p->name);
		strrevcat(file_name, root_directory);
		strcpy(file_name, root_directory);
		//kprintf(" name=%s ", file_name);
		//kprintf("mode=%s ", oct2bin((unsigned char *)p->mode, 8));
		//kprintf("linkname=%s ", p->linkname);
		//kprintf("uname=%s ", p->uname);
		//kprintf("prefix=%s  ", p->prefix);
		//kprintf("devmajor=%s  ", p->devmajor);
		//kprintf("devminor=%s  ", p->devminor);
		//kprintf("uid=%s ", oct2bin((unsigned char *)p->uid, 8));
		//kprintf("gid=%s ", oct2bin((unsigned char *)p->gid, 8));
		//kprintf("size=%p  ", p->size);
		//int filesize = oct2bin((unsigned char *)p->size, 11);
		
		//kprintf("size=%d ", file_size);
		//kprintf("typeflag=%c ", file_typeflag);
		//kprintf("header=%p", file_tarfs_header);
		//kprintf("\n");
		//p++;
		//if(file_size == 0) {
		uint64_t address = (uint64_t)p + file_size;
		kprintf("address= %p\n", address);
		switch(file_typeflag) {
		int parent_index;
		case '0':;
		  parent_index = get_parent_index(file_name, 1);
		  //kprintf("Parent index before inserting file is %d\n", parent_index);
		  assign_vfs_slot(vfs_pointer, file_name, parent_index, file_size, (int)file_typeflag, address);
		  vfs_pointer++;
		  break;
		case '5':;
		  parent_index = get_parent_index(file_name, 2);
		  //kprintf("Parent index before inserting file is %d\n", parent_index);
		  assign_vfs_slot(vfs_pointer, file_name, parent_index, file_size, (int)file_typeflag, address);
		  vfs_pointer++;
		  break;

		}  
		//}
		file_number = (((file_size + 511) / 512) + 1);
		//to skip the file contents
		if(file_typeflag == 0) {
		  file_number += 1;
		}
		p += file_number;
		//file_number += (((file_size + 511) / 512) + 1) * 512;
	}
}

void print_vfs() {
	for(int i = 0; i < vfs_pointer; i++) {
	  kprintf("Index = %d, Name = %s, ParentIndex = %d, type = %c\n", i, vfs[i].name, vfs[i].parent);
	  //kprintf("Name = %s, address = %p\n", vfs[i].name, vfs[i].address);
	}
}

uint64_t tar_lookup(char *filename) {
  int index = 0;
  while(strcmp(vfs[index].name, filename) != 0 && index < vfs_pointer) {
    index++;
  }
  if(index < vfs_pointer) {
    return vfs[index].address;
  }
  return (uint64_t)NULL;
  
}

int tar_get_index(char *filename) {
  int index = 0;
  while(strcmp(vfs[index].name, filename) != 0 && index < vfs_pointer) {
    index++;
  }
  if(index < vfs_pointer) {
    return index;
  }
  return -1;
  
}

uint64_t tar_ls(char *filename) {
  int parent_index = tar_get_index(filename);
  int index = 0;
  int file_count = 0;
  while(index < vfs_pointer) {
    if(vfs[index].parent == parent_index) {
      kprintf("%d %s\n", file_count, vfs[index].name);
      /*if(file_count < 2) {
      strcpy(files[file_count], vfs[index].name);
      }
      file_count++;
      if(file_count == 3) {
	break;
	}*/
      file_count++;
    }
    index++;
  }
  return file_count;

}
