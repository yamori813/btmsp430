# This is mruby script

sp = SerialPort.new("/dev/cuaU0", 9600, 8, 1, 0)

# SONY PT-D4W B ON
cmd = [0x32, 0x42, 0x30]

hex_array = cmd.map { |n| sprintf('%02x', n) }

p hex_array.join

sp.write hex_array.join + "\n"

res = ""
loop do
  c = sp.getc
  res += c
  if c == "\n" then
    break
  end
end

p res

