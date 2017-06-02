/*
SQLyog Community v12.2.0 (64 bit)
MySQL - 10.1.9-MariaDB : Database - distribuidosdb
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`distribuidosdb` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `distribuidosdb`;

/*Table structure for table `buz_col` */

DROP TABLE IF EXISTS `buz_col`;

CREATE TABLE `buz_col` (
  `id_buz` int(10) NOT NULL AUTO_INCREMENT,
  `id_col` int(10) NOT NULL,
  PRIMARY KEY (`id_buz`,`id_col`),
  KEY `FK_buzcol_col` (`id_col`),
  CONSTRAINT `FK_buzcol_buz` FOREIGN KEY (`id_buz`) REFERENCES `buzon` (`id_buz`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `FK_buzcol_col` FOREIGN KEY (`id_col`) REFERENCES `cola` (`id_col`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

/*Data for the table `buz_col` */

insert  into `buz_col`(`id_buz`,`id_col`) values 
(1,1),
(1,2),
(1,3);

/*Table structure for table `buzon` */

DROP TABLE IF EXISTS `buzon`;

CREATE TABLE `buzon` (
  `id_buz` int(10) NOT NULL AUTO_INCREMENT,
  `tip_buz` smallint(1) NOT NULL,
  `nom_buz` char(20) NOT NULL,
  PRIMARY KEY (`id_buz`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

/*Data for the table `buzon` */

insert  into `buzon`(`id_buz`,`tip_buz`,`nom_buz`) values 
(1,2,'tipo2');

/*Table structure for table `cliente` */

DROP TABLE IF EXISTS `cliente`;

CREATE TABLE `cliente` (
  `id_cli` char(12) NOT NULL,
  `clave` char(10) NOT NULL,
  PRIMARY KEY (`id_cli`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Data for the table `cliente` */

insert  into `cliente`(`id_cli`,`clave`) values 
('c1','1'),
('c2','1'),
('c3','1'),
('c4','1'),
('c5','1'),
('c6','1');

/*Table structure for table `col_cli` */

DROP TABLE IF EXISTS `col_cli`;

CREATE TABLE `col_cli` (
  `id_col` int(10) NOT NULL AUTO_INCREMENT,
  `id_cli` char(12) NOT NULL,
  PRIMARY KEY (`id_col`,`id_cli`),
  KEY `FK_colcli_cli` (`id_cli`),
  CONSTRAINT `FK_colcli_cli` FOREIGN KEY (`id_cli`) REFERENCES `cliente` (`id_cli`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `FK_colcli_col` FOREIGN KEY (`id_col`) REFERENCES `cola` (`id_col`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

/*Data for the table `col_cli` */

insert  into `col_cli`(`id_col`,`id_cli`) values 
(1,'c1'),
(1,'c2'),
(1,'c5'),
(2,'c1'),
(2,'c2'),
(2,'c3'),
(2,'c4');

/*Table structure for table `cola` */

DROP TABLE IF EXISTS `cola`;

CREATE TABLE `cola` (
  `id_col` int(10) NOT NULL AUTO_INCREMENT,
  `nom_col` char(20) NOT NULL,
  PRIMARY KEY (`id_col`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

/*Data for the table `cola` */

insert  into `cola`(`id_col`,`nom_col`) values 
(1,'cl1'),
(2,'cl2'),
(3,'cl3');

/*Table structure for table `msj` */

DROP TABLE IF EXISTS `msj`;

CREATE TABLE `msj` (
  `id_msj` int(10) NOT NULL AUTO_INCREMENT,
  `id_col` int(10) NOT NULL,
  `cont_msj` char(250) NOT NULL,
  PRIMARY KEY (`id_msj`,`id_col`),
  KEY `FK_msj_cola` (`id_col`),
  CONSTRAINT `FK_msj_cola` FOREIGN KEY (`id_col`) REFERENCES `cola` (`id_col`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Data for the table `msj` */

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
