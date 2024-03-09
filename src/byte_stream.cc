#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return is_closed_;
}

void Writer::push( string data )
{
  // Your code here.
  auto allowed = this->available_capacity();
  if ( allowed < data.length() ) {
    data = data.substr( 0, allowed );
  }
  buffer += data;
  bytes_pushed_ += data.length();
}

void Writer::close()
{
  // Your code here.
  is_closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - buffer.length();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return is_closed_ && ( ( bytes_popped_ == bytes_pushed_ ) && buffer == "" );
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_;
}

string_view Reader::peek() const
{
  // Your code here.
  return buffer;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if ( len > bytes_buffered() ) {
    len = bytes_buffered();
  }

  auto popped = buffer.substr( 0, len );
  buffer = buffer.substr( len, bytes_buffered() );
  bytes_popped_ += len;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffer.length();
}
