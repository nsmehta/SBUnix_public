#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <sys/elf64.h>

void assign_vfs_slot(int nindex, char nname[128], int nparent, uint64_t nsize, int ntype, uint64_t naddress, int noffset) {
	strcpy(vfs[nindex].name, nname);
	vfs[nindex].parent = nparent;
	vfs[nindex].size = nsize;
	vfs[nindex].type = ntype;
	vfs[nindex].address = naddress;
	vfs[nindex].index = nindex;
	vfs[nindex].offset = noffset;
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

  assign_vfs_slot(0, "/", -1, 0, 5, 0, 0);
  assign_vfs_slot(1, "/rootfs/", 0, 0, 5, 0, 512);
	vfs_pointer = 2;
	char *root_directory_name = "/rootfs/";
	int offset = 512;
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
		kprintf(" name=%s ", file_name);
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
		//kprintf("file_size/512: %d %d\n", file_size, file_size%512);
		//kprintf("offset = %d", offset);
		uint64_t address = (uint64_t)p + file_size;
		kprintf("address= %p\n", address);
		if(file_size == 0 || file_size % 512 == 0) {
		  offset += file_size + 512;
		}
		else {
		  offset += (file_size + (512 - file_size % 512) + 512);
		}

		switch(file_typeflag) {
		int parent_index;
		case '0':;
		  parent_index = get_parent_index(file_name, 1);
		  //kprintf("Parent index before inserting file is %d\n", parent_index);
		  assign_vfs_slot(vfs_pointer, file_name, parent_index, file_size, (int)file_typeflag, address, offset);
		  vfs_pointer++;
		  break;
		case '5':;
		  parent_index = get_parent_index(file_name, 2);
		  //kprintf("Parent index before inserting file is %d\n", parent_index);
		  assign_vfs_slot(vfs_pointer, file_name, parent_index, file_size, (int)file_typeflag, address, offset);
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
	  kprintf("Index = %d, Name = %s, ParentIndex = %d, size = %d, offset = %d\n", i, vfs[i].name, vfs[i].parent, vfs[i].size, vfs[i].offset);
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

/*
int get_file_offset(char *filename) {
  char root_directory_name[9] = "/rootfs/";
  struct posix_header_ustar *p = (struct posix_header_ustar *)&_binary_tarfs_start;
  int offset = 0;
  int file_number = 0;
  kprintf("filename = %s\n", filename);
  while(p <= (struct posix_header_ustar *)&_binary_tarfs_end && strlen(p->name) != 0) {
    //kprintf("p->name = %s  ", p->name);

    char p_filename[128];
    char *root_directory = "";
    strcpy(root_directory, root_directory_name);

    strcpy(p_filename, p->name);
    strrevcat(p_filename, root_directory);
    strcpy(p_filename, root_directory);
    //kprintf(" p_filename=%s ", p_filename);


    if(strcmp(filename, p_filename) == 0) {
      return offset;
    }
    uint64_t file_size = oct2bin((unsigned char *)p->size, 11);
    if(file_size == 0 || file_size % 512 == 0) {
      offset += file_size + 512;
    }
    else {
      offset += (file_size + (512 - file_size % 512) + 512);
    }
    //kprintf("offset = %d\n", offset);
    char file_typeflag = p->typeflag[0];
    file_number = (((file_size + 511) / 512) + 1);
    //to skip the file contents
    if(file_typeflag == 0) {
      file_number += 1;
    }
    //p += file_number;
    p = (struct posix_header_ustar *)(&_binary_tarfs_start + offset);
  }
  return 0;

}
*/

int get_file_offset(char *filename) {
  char root_directory_name[9] = "/rootfs/";
  struct posix_header_ustar *p = (struct posix_header_ustar *)&_binary_tarfs_start;
  int offset = 0;
  int file_number = 0;
  kprintf("filename = %s\n", filename);
  while(p <= (struct posix_header_ustar *)&_binary_tarfs_end) { // && strlen(p->name) != 0) {
    //kprintf("p->name = %s  ", p->name);

    char p_filename[128];
    char *root_directory = "";
    strcpy(root_directory, root_directory_name);

    strcpy(p_filename, p->name);
    strrevcat(p_filename, root_directory);
    strcpy(p_filename, root_directory);
    //kprintf(" p_filename=%s ", p_filename);

    /*
    if(strcmp(filename, p_filename) == 0) {
      return offset;
    }
    */
    uint64_t file_size = oct2bin((unsigned char *)p->size, 11);
    /*    if(file_size == 0 || file_size % 512 == 0) {
      offset += file_size + 512;
    }
    else {
      offset += (file_size + (512 - file_size % 512) + 512);
      }*/

    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) (&_binary_tarfs_start + offset);
    if(elf_header->e_ident[0] == 0x7f) {
  		kprintf("e_ident[0] = 0x%x%c%c%c, offset = %d\n", elf_header->e_ident[0], elf_header->e_ident[1], elf_header->e_ident[2], elf_header->e_ident[3], offset);
    }
    

    offset += 1;
    //kprintf("offset = %d\n", offset);
    char file_typeflag = p->typeflag[0];
    file_number = (((file_size + 511) / 512) + 1);
    //to skip the file contents
    if(file_typeflag == 0) {
      file_number += 1;
    }
    //p += file_number;
    p = (struct posix_header_ustar *)(&_binary_tarfs_start + offset);
  }
  return 0;

}


void get_file_content(char *filename) {
  //int file_index = tar_get_index(filename);
  //int file_offset = file_index * 512 + 512;
  //int file_offset = vfs[file_index].offset;
  int file_offset = test_offset(filename);
  kprintf("file_offset = %d\n", file_offset);
  Elf64_Ehdr *elf_header = (Elf64_Ehdr *) (&_binary_tarfs_start + file_offset);
      /*kprintf("e_ident[0] = %x ", elf_header->e_ident[0]);
  kprintf("e_ident[1] = %c ", elf_header->e_ident[1]);
  kprintf("e_ident[2] = %c ", elf_header->e_ident[2]);
  kprintf("e_ident[3] = %c\n", elf_header->e_ident[3]);
  
  //file_offset = vfs[file_index].offset + 512;

  file_offset += 512;
  kprintf("file_offset = %d\n", file_offset);
  elf_header = (Elf64_Ehdr *) (&_binary_tarfs_start + file_offset);
  kprintf("e_ident[0] = %x ", elf_header->e_ident[0]);
  kprintf("e_ident[1] = %c ", elf_header->e_ident[1]);
  kprintf("e_ident[2] = %c ", elf_header->e_ident[2]);
  kprintf("e_ident[3] = %c\n", elf_header->e_ident[3]);
  */
  file_offset += 1024;
  kprintf("file_offset = %d\n", file_offset);
  elf_header = (Elf64_Ehdr *) (&_binary_tarfs_start + file_offset);
  kprintf("e_ident[0] = %x ", elf_header->e_ident[0]);
  kprintf("e_ident[1] = %c ", elf_header->e_ident[1]);
  kprintf("e_ident[2] = %c ", elf_header->e_ident[2]);
  kprintf("e_ident[3] = %c\n", elf_header->e_ident[3]);
  

/*
  file_offset = vfs[file_index].offset - 512;
  kprintf("file_offset = %d\n", file_offset);
  elf_header = (Elf64_Ehdr *) (&_binary_tarfs_start + file_offset);
  kprintf("e_ident[0] = %x ", elf_header->e_ident[0]);
  kprintf("e_ident[1] = %c ", elf_header->e_ident[1]);
  kprintf("e_ident[2] = %c ", elf_header->e_ident[2]);
  kprintf("e_ident[3] = %c\n", elf_header->e_ident[3]);

  file_offset = vfs[file_index].offset + 512 + 512;
  kprintf("file_offset = %d\n", file_offset);
  elf_header = (Elf64_Ehdr *) (&_binary_tarfs_start + file_offset);
  kprintf("e_ident[0] = %x ", elf_header->e_ident[0]);
  kprintf("e_ident[1] = %c ", elf_header->e_ident[1]);
  kprintf("e_ident[2] = %c ", elf_header->e_ident[2]);
  kprintf("e_ident[3] = %c\n", elf_header->e_ident[3]);
  */
}

int test_offset(char *filename) {
	struct posix_header_ustar *p = (struct posix_header_ustar *) &_binary_tarfs_start;
	int offset = 512;
	while(strlen(p->name) != 0) {
		p = (struct posix_header_ustar *) (&_binary_tarfs_start + offset);
		uint64_t file_size = oct2bin((unsigned char *)p->size, 11);
		if(strcmp(p->name, filename) >= 0) {
			return offset + 512;
		}
		if(file_size == 0) {
			offset += 512;
		} else {
			offset += (file_size % 512 == 0) ? file_size + 512 : file_size + (512 - file_size % 512) + 512;
		}
	}
	return 0;
}
