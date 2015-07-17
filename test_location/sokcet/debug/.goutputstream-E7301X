OBJS=*.o
ODIR=obj
$(ROOT_DIR)/$(BIN_DIR)/$(BIN):$(ODIR)/$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	cp $(ROOT_DIR)/$(BIN_DIR)/$(BIN)  $(ROOT_DIR)/$(ROOT_DIR_BIN)
