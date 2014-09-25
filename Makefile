MAKEDEPEND = /usr/bin# /cpp

DEPDIR = .deps
df = $(DEPDIR)/$(*F)

TARGET = mom

all: $(TARGET)

LIBS = -lreadline -lhistory -lcurses
CFLAGS = -Wall -g

CXXFILES = \
	main.cxx \
	App.cxx \
	Util.cxx \
	I2CDriver.cxx \
	Processor.cxx \
	Session.cxx \
	Help.cxx \
	Show.cxx \
	Set.cxx \
	Send.cxx \
	Ping.cxx

OFILES = $(CXXFILES:.cxx=.o)

-include $(OFILES:.o=.d)

%.o: %.cxx
	c++ -c -o $@ $< $(CFLAGS)
	c++ -MM $(CFLAGS) $< > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

$(TARGET): $(OFILES)
	c++ -o $(TARGET) $(OFILES) $(LIBS)
	sudo chown root:root $(TARGET)
	sudo chmod u+s $(TARGET)

clean:
	rm -f $(TARGET) *.o *~ core *.d
