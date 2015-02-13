#include "bootimagemanager.h"
#include "bootimage.h"
#include "utils.h"
#include "QCryptographicHash"

#include "QDebug"

#include <elfio/elfio.hpp>
using namespace ELFIO;

unsigned getNextBlock(unsigned pagesize,unsigned itemsize)
{
    unsigned pagemask = pagesize - 1;
    unsigned count;

    if((itemsize & pagemask) == 0) {
        return itemsize;
    }

    count = pagesize - (itemsize & pagemask);

    return count + itemsize;

}

bool BootImageManager::openBootImageFormat(QString fileName)
{
    QByteArray data = Utils::loadFile(fileName);
    if(data.isEmpty())
        return false;

    const char* data_ptr = data.constData();

    boot_img_hdr header;
    memcpy(&header,data_ptr,sizeof(boot_img_hdr));

    Utils::addLog("Checking if valid bootimage");
    if(strncmp((char*)header.magic,BOOT_MAGIC,BOOT_MAGIC_SIZE) != 0)
    {
        Utils::addLog("Not a valid android boot image");
        return false;
    }

    Utils::addLog("Processing Android boot image");

    m_pageSize = header.page_size;
    m_baseAddress = DEFAULT_BASE_ADDRESS;
    m_kernelAddress = header.kernel_addr;
    m_secondAddress = header.second_addr;
    m_ramdiskAddress = header.ramdisk_addr;
    m_tagsOffset    = header.tags_addr;

    Utils::addLog("Page Size Offset: 0x%08X",m_pageSize);
    Utils::addLog("Kernel Offset: 0x%08X",m_kernelAddress);
    Utils::addLog("Ramdisk Offset: 0x%08X",m_ramdiskAddress);
    Utils::addLog("SecondData Offset: 0x%08X",m_secondAddress);
    Utils::addLog("Tags Offset: 0x%08X",m_tagsOffset);






    unsigned currentItem = getNextBlock(header.page_size,sizeof(header));

    Utils::addLog("<b>CommandLine :</b>\n%s",header.cmdline);
    m_commandLine = QString((char* )header.cmdline);

    if(header.cmdline[BOOT_ARGS_SIZE -2] != 0)
        m_commandLine.append((char*)header.extra_cmdline);


    Utils::addLog("Found kernel [0x%X bytes]",header.kernel_size);

    m_kernelData = QByteArray(data_ptr + currentItem,header.kernel_size);

    currentItem = currentItem + getNextBlock(header.page_size,header.kernel_size);

    Utils::addLog("Found Ramdisk [0x%X bytes] ",header.ramdisk_size);



    m_ramdiskData = QByteArray(data_ptr + currentItem,header.ramdisk_size);

    currentItem = currentItem + getNextBlock(header.page_size,header.ramdisk_size);

    if(header.second_size > 0)
    {
        Utils::addLog("Found Second Image [0x%X bytes]",header.second_size);

        m_secondData = QByteArray(data_ptr + currentItem,header.second_size);

        currentItem = currentItem +  getNextBlock(header.page_size,header.second_size);
    }


    Utils::addLog("Found Device Tree [0x%X bytes]",header.dt_size);

    m_deviceTreeData = QByteArray(data_ptr + currentItem,header.dt_size);

    Utils::addLog("Done");

    return true;

}

bool BootImageManager::openElfFormat(QString fileName)
{
    elfio reader;

//    Utils::addLog("Checking if ELF file ");
    // Load ELF data
    if ( !reader.load( fileName.toStdString() ) ) {
        /* not an elf file*/
        Utils::addLog("Not a valid elf file");
        return false;
    }

    Utils::addLog("Processing ELF file");


    m_kernelAddress = DEFAULT_KERNEL_OFFSET;
    m_pageSize = DEFAULT_PAGE_SIZE;
    m_ramdiskAddress = DEFAULT_RAMDISK_OFFSET;
    m_secondAddress = DEFAULT_SECOND_OFFSET;
    m_tagsOffset = DEFAULT_TAGS_OFFSET;



    // Print ELF file sections info
    Elf_Half sec_num = reader.sections.size();
    Utils::addLog("Number of sections: %d", sec_num);
    if(sec_num >= 1)
    {
        section* psec = reader.sections[0];
        char * extraData = (char*)psec->get_data();
        int extraDataSize  = psec->get_size();
        quint32 *val = (quint32*)extraData;
        val++;
        m_tagsOffset = *val;


        m_commandLine = QByteArray::fromRawData((extraData +8),
                               (extraDataSize  - 8));
        Utils::addLog("<b>CommandLine data: </b>\n%s",m_commandLine.toLocal8Bit().data());
        Utils::addLog("TagsOffset: 0x%08X",m_tagsOffset);


    }

    /* reset all data*/
    m_ramdiskData = m_secondData = m_deviceTreeData = m_kernelData = QByteArray();

    Elf_Half seg_num = reader.segments.size();

    segment* pseg;
    /* save kernel*/
    if(seg_num >= 1)
    {
        pseg = reader.segments[0];
        m_kernelData = QByteArray(pseg->get_data(),pseg->get_file_size());
        m_kernelAddress = pseg->get_physical_address();
        Utils::addLog("Found Kernel [0x%X bytes]",m_kernelData.size());
        Utils::addLog("Kernel Offset: 0x%08X",m_kernelAddress);


    }

    if(seg_num >= 2)
    {
        pseg = reader.segments[1];
        m_ramdiskData = QByteArray(pseg->get_data(),pseg->get_file_size());
        m_ramdiskAddress = pseg->get_physical_address();
        Utils::addLog("Found Ramdisk [0x%X bytes]",m_ramdiskData.size());
        Utils::addLog("Ramdisk Offset: 0x%08X",m_ramdiskAddress);
    }

    if(seg_num >= 3)
    {
        pseg = reader.segments[2];
        m_deviceTreeData = QByteArray(pseg->get_data(),pseg->get_file_size());
        Utils::addLog("Found Device Tree [0x%X bytes]",m_deviceTreeData.size());

    }

    if(seg_num >= 4)
    {
        pseg = reader.segments[3];
        m_secondData = QByteArray(pseg->get_data(),pseg->get_file_size());
        m_secondAddress = pseg->get_physical_address();
        Utils::addLog("Found Second Data [0x%X bytes]",m_secondData.size());
        Utils::addLog("SecondData Offset: 0x%08X",m_secondAddress);
    }



    return true;

}

BootImageManager::BootImageManager()
{

    m_pageSize = DEFAULT_PAGE_SIZE;
    m_baseAddress = DEFAULT_BASE_ADDRESS;
    m_commandLine = QString("");
    m_kernelAddress = DEFAULT_KERNEL_OFFSET;
    m_ramdiskAddress = DEFAULT_RAMDISK_OFFSET;
    m_secondAddress = DEFAULT_SECOND_OFFSET;
    m_tagsOffset = DEFAULT_TAGS_OFFSET;

}

BootImageManager::~BootImageManager()
{

}

bool BootImageManager::Open(const QString fileName)
{

    if(openBootImageFormat(fileName) == false)
        return openElfFormat(fileName);

    return true;

}

static ssize_t getPaddingSize(quint32 pageSize, quint32 itemSize)
{
    unsigned pagemask = pageSize - 1;
    ssize_t count;
    if((itemSize & pagemask) == 0) {
        return 0;
    }

    count = pageSize - (itemSize & pagemask);

    return count;
}


static QByteArray generatePadding(quint32 pageSize,quint32 itemSize)
{
    QByteArray padding;
    padding.fill(0,getPaddingSize(pageSize,itemSize));
    return padding;
}




#include "QBuffer"

QByteArray BootImageManager::packBootImage()
{
    struct boot_img_hdr hdr;

    QByteArray result;

    QString boardName("\0");

    QBuffer outBuffer(&result);

    if(m_kernelData.size() <= 0 )
    {
        set_errorString("Kernel data not loaded.");
        return QByteArray();
    }

    QCryptographicHash sha1(QCryptographicHash::Sha1);
    sha1.reset();


    Utils::addLog("Starting build of bootimage");

    memset(&hdr, 0, sizeof(hdr));


    hdr.page_size = m_pageSize;

    hdr.kernel_addr =  m_baseAddress + m_kernelAddress;
    hdr.ramdisk_addr = m_baseAddress + m_ramdiskAddress;
    hdr.second_addr =  m_baseAddress + m_secondAddress;
    hdr.tags_addr =    m_baseAddress + m_tagsOffset;
    hdr.kernel_size = m_kernelData.size();
    hdr.ramdisk_size = m_ramdiskData.size();
    hdr.dt_size  = m_deviceTreeData.size();
    hdr.second_size = m_secondData.size();

    strcpy((char*)hdr.name, boardName.toLocal8Bit().data());

    memcpy(hdr.magic, BOOT_MAGIC, BOOT_MAGIC_SIZE);

    strncpy((char *)hdr.cmdline, m_commandLine.toLocal8Bit().data(), BOOT_ARGS_SIZE - 1);
    hdr.cmdline[BOOT_ARGS_SIZE - 1] = '\0';

    if (m_commandLine.length() >= (BOOT_ARGS_SIZE - 1)) {

        Utils::addLog("Command Line larger than %d, using extra args",BOOT_ARGS_SIZE);
        strncpy((char *)hdr.extra_cmdline,
                m_commandLine.toLocal8Bit().data() + (BOOT_ARGS_SIZE-1) ,
                BOOT_EXTRA_ARGS_SIZE);
    }





    /* put a hash of the contents in the header so boot images can be
     * differentiated based on their first 2k.
     */

    sha1.addData( (char*)m_kernelData.data(), hdr.kernel_size);
    sha1.addData( (char*)&hdr.kernel_size, sizeof(hdr.kernel_size));
    sha1.addData( (char*)m_ramdiskData.data(), hdr.ramdisk_size);
    sha1.addData( (char*)&hdr.ramdisk_size, sizeof(hdr.ramdisk_size));
    sha1.addData( (char*)m_secondData.data(), hdr.second_size);
    sha1.addData( (char*)&hdr.second_size, sizeof(hdr.second_size));

    if (m_deviceTreeData.isEmpty() == false) {
        sha1.addData( (char*)m_deviceTreeData.data(), hdr.dt_size);
        sha1.addData( (char*)&hdr.dt_size, sizeof(hdr.dt_size));
    }

    QByteArray sha1_result = sha1.result();
    Utils::addLog("Computed SHA1 checksum : %s", QString(sha1_result.toHex()).toLocal8Bit().data());


    memcpy((char*)hdr.id,
           sha1_result.data(),
           sha1_result.size() > (int) sizeof(hdr.id) ? sizeof(hdr.id) : sha1_result.size());


//    outBuffer.setFileName(m_outputFileName);
    outBuffer.open(QIODevice::WriteOnly);
    outBuffer.seek(0);

    if(outBuffer.isWritable() == false)
    {
        set_errorString("Error allocating memory for output buffer");
        return QByteArray();
    }



    Utils::addLog("Writing Boot Header[%d bytes]",sizeof(hdr));
    outBuffer.write((char*)&hdr,sizeof(hdr));
    Utils::addLog("Writing Padding [%d bytes]",getPaddingSize(get_pageSize(),sizeof(hdr)));
    outBuffer.write(generatePadding(get_pageSize(),sizeof(hdr)));


    Utils::addLog("Writing Kernel [%d bytes]",m_kernelData.size());
    outBuffer.write(m_kernelData);
    Utils::addLog("Writing Padding [%d bytes]",getPaddingSize(get_pageSize(),sizeof(hdr)));
    outBuffer.write(generatePadding(get_pageSize(),m_kernelData.size()));



    Utils::addLog("Writing Ramdisk [%d bytes]",m_ramdiskData.size());
    outBuffer.write(m_ramdiskData);
    Utils::addLog("Writing Padding [%d bytes]",getPaddingSize(get_pageSize(),m_ramdiskData.size()));
    outBuffer.write(generatePadding(get_pageSize(),m_ramdiskData.size()));

    if(m_secondData.isEmpty() == false) {
        Utils::addLog("Writing Second image [%d bytes]",m_secondData.size());
        outBuffer.write(m_secondData);
        Utils::addLog("Writing Padding [%d bytes]",getPaddingSize(get_pageSize(),m_secondData.size()));
        outBuffer.write(generatePadding(get_pageSize(),m_secondData.size()));

    }

    if (m_deviceTreeData.isEmpty() == false) {
        Utils::addLog("Writing Device Tree [%d bytes]",m_deviceTreeData.size());
        outBuffer.write(m_deviceTreeData);
        Utils::addLog("Writing Padding [%d bytes]",getPaddingSize(get_pageSize(),m_deviceTreeData.size()));
        outBuffer.write(generatePadding(get_pageSize(),m_deviceTreeData.size()));
    }

    Utils::addLog("Bootimage Build Complete");

    outBuffer.close();

    return result;

}


/* a copy constructor. This is just in case we want to add extra features here. as of now this does
nothing other than the default copy constructor  */

BootImageManager &BootImageManager::operator=(const BootImageManager b)
{
    this->m_baseAddress = b.m_baseAddress;
    this->m_commandLine = b.m_commandLine;
    this->m_deviceTreeData  = b.m_deviceTreeData;
    this->m_errorString = b.m_errorString;
    this->m_kernelAddress  = b.m_kernelAddress;
    this->m_kernelData = b.m_kernelData;
    this->m_pageSize = b.m_pageSize;
    this->m_ramdiskAddress = b.m_ramdiskAddress;
    this->m_ramdiskData = b.m_ramdiskData;
    this->m_secondAddress = b.m_secondAddress;
    this->m_secondData = b.m_secondData;
    this->m_tagsOffset = b.m_tagsOffset;

    return *this;
}

