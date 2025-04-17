# PostgreSQL Example Project

## VERSION
v1.0.0 - Initial MVP.
v1.0.1 - Added mosaic result table.
v1.0.3 - Not building the default executable by default.
v1.0.4 - Installing the libraries to the correct spot.
v1.0.5 - Not building the default executable by default.
v1.0.6 - Fixing include path.
v1.0.7 - Installing the libraries to the correct spot.
v1.0.8 - Installing the libraries to the correct spot.
v1.0.9 - Added a variable to include the directories.

## Create Release
```bash
 VERSION=v1.0.9
 git add .
 git commit -m "Release ${VERSION}"
 git push
 git tag -a ${VERSION} -m "Release ${VERSION}"
 git push origin ${VERSION}
```

This project demonstrates how to connect to a PostgreSQL database using C++ and `libpq`.

## Prerequisites

- **C++ Compiler**: Supports C++14 or later.
- **CMake**: Version 3.10 or later.
- **PostgreSQL**: Installed and running.
- **libpq**: PostgreSQL client library.

### Installation

#### macOS

```bash
brew install postgresql
```

#### Ubuntu
```bash
sudo apt-get update
sudo apt-get install libpq-dev
```

### Setup
#### Clone the Repository:
```bash
git clone https://github.com/yourusername/postgresql-example.git
cd postgresql-example
```

#### Set Environment Variable:
```bash
export DB_CONN_STRING="dbname=mydatabase user=yourusername password=yourpassword hostaddr=127.0.0.1 port=5432"
```

#### Build the Project:
```bash
export DB_CONN_STRING="dbname=mydatabase user=yourusername password=yourpassword hostaddr=127.0.0.1 port=5432"
mkdir build
cd build
cmake ..
make
```

### Usage
Reset Tables:
```bash
./PostgreSQLExample --reset
```

Create Tables:
```bash
./PostgreSQLExample
```

### Notes
Security: Store credentials securely.
Error Handling: Basic error handling included.
License
MIT License.

### Contact
For questions, contact yourname@domain.com.

### Customization

- **Repository URL**: Replace `https://github.com/yourusername/postgresql-example.git` with your actual repository URL.
- **Contact Information**: Update the email address with your actual contact information.

