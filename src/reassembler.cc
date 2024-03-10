#include "reassembler.hh"
#include <iostream>

using namespace std;

bool is_overlapped( const ReassembleItem& a, const ReassembleItem& b )
{
  if ( a.start > b.end ) {
    return false;
  }

  if ( a.end < b.start ) {
    return false;
  }

  return true;
}

void Reassembler::insert_m( ReassembleItem& item )
{
  auto it = m.lower_bound( item.start );

  while ( it != m.begin() ) {
    auto prevIt = std::prev( it );
    auto prevItem = prevIt->second;
    if ( !is_overlapped( prevItem, item ) )
      break;

    // merge
    auto s = prevItem.start;
    auto e = prevItem.end;

    if ( item.end <= e ) {
      item.data = prevItem.data;
      item.end = e;
    } else {
      item.data = prevItem.data.substr( 0, item.start - s ) + item.data;
    }
    item.start = s;

    m.erase( prevIt );
  }

  while ( it != m.end() ) {
    if ( !is_overlapped( it->second, item ) )
      break;

    // merge
    auto s = it->second.start;
    auto e = it->second.end;

    if ( item.end < e ) {
      item.data = item.data.substr( 0, s - item.start ) + it->second.data;
      item.end = e;
    }

    it = m.erase( it );
  }

  // merging finished, insert
  m[item.start] = item;
}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  if(is_last_substring){
    last = first_index + data.length();
  }

//  uint64_t l = 0;
//  long n = data.length();
  ReassembleItem item;
  item.start = first_index;
  item.end = first_index + data.length();

  if ( first_index <= cur ) {
//    l = cur - first_index;
//    n -= cur - first_index;
    item.start = cur;
  }

  if ( first_index + data.length() > cur + writer().available_capacity() ) {
//    auto diff = ( first_index + data.length() ) - ( cur + writer().available_capacity() );
//    n -= diff;
    item.end = cur + writer().available_capacity();
  }


  if ( item.end - item.start <= 0 ) {
    // discard
    if ( cur == last) {
      output_.writer().close();
    }
    return;
  }

//  data = data.substr( l, n );
  data = data.substr( item.start - first_index, item.end - item.start );

  // insert to buffer
  item.data = data;

  insert_m( item );

  // try to push as much as possible
  auto it = m.begin();
  while ( it != m.end() ) {
    if ( it->second.start > cur ) {
      break;
    }

    // push
    output_.writer().push( it->second.data );
    cur += it->second.data.length();
    it = m.erase( it );
  }

  if ( cur == last) {
    output_.writer().close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  auto len = 0;
  auto it = m.begin();
  while ( it != m.end() ) {
    len += it->second.data.length();
    it++;
  }

  return len;
}
