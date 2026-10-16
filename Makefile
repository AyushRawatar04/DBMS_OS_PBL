CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra \
           -I/opt/homebrew/Cellar/mysql/26.7.0_1/include
LDFLAGS  = -L/opt/homebrew/Cellar/mysql/26.7.0_1/lib \
           -L/opt/homebrew/lib \
           -lmysqlclient -lz -lssl -lcrypto -lresolv

TARGET   = TicketBookingSystem

SRCS     = Main.cpp \
           Booking.cpp \
           BookingService.cpp \
           Cinema.cpp \
           Customer.cpp \
           DBConnection.cpp \
           Logger.cpp \
           Movie.cpp \
           Payment.cpp \
           PriceCalculator.cpp \
           Screen.cpp \
           Seat.cpp \
           Show.cpp \
           ShowSeat.cpp \
           TicketPrinter.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "\n✅  Build successful — run with: ./$(TARGET)\n"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
