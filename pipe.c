/*
 * This file is part of PRO ONLINE.

 * PRO ONLINE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO ONLINE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO ONLINE. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pipe.h>
#include <config.h>
#include <stdio.h>

#include <string.h>    // strlen
#include <sys/types.h> // mkfifo()
#include <sys/stat.h>  // open()
#include <fcntl.h>     // open(), O_NONBLOCK
#include <unistd.h>    // write(), close()
#include <errno.h>     // perror

int pd = -1;

int create_pipe()
{
  if (mkfifo(PIPE_NAME, 0666) == -1) // read/write for all
  {
    if (errno != EEXIST)
    {
      perror("Failed to create pipe");
      return -1;
    }
  }
  if (pd != -1)
  {
    close_pipe(); // just in case
  }
  pd = open(PIPE_NAME, O_NONBLOCK|O_WRONLY); // non-blocking and write only
  if(pd == -1)
  {
    if(errno != ENXIO) // need to wait for read to be opened
    {
      perror("Failed to open pipe");
      return -1;
    }
    return -2; // return -2 so we don't kill ourself in main.c, allows us to try again later
  }
  return pd;
}

void close_pipe()
{
  if (pd > 0)
  {
    close(pd);
    pd = -1;
  }
}

int write_pipe(const char* msg)
{
  int sent = strlen(msg);
  if (pd < 0)
    create_pipe();
  sent = write(pd, msg, sent + 1);
  return sent;
}
